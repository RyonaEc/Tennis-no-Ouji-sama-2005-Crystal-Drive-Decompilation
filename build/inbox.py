"""Collect the scout's report and gate its claims before anything is installed.

The scout works in a separate worktree (M:/ATOJ-scout) and is not allowed to
install. It writes `MATCH <mod> <addr> <size> <name> <path>` lines into
build/cand/scout-<band>.md there. This reads those reports, recompiles every
claimed candidate in *this* tree, and only hands verified ones to t10.try_unit,
so a scout bug or a stale band can never reach delinks.txt.

Run from the repository root.

Usage:
    python build/inbox.py                 # what has the scout found
    python build/inbox.py --install       # install the verified claims
    python build/inbox.py --next "text"   # give the scout a new band
"""
import argparse
import pathlib
import re
import shutil
import subprocess

SCOUT = pathlib.Path("M:/ATOJ-scout")
WORKER = pathlib.Path("M:/ATOJ-worker")
DELTA = pathlib.Path("M:/ATOJ-delta")
REPORTS = SCOUT / "build/cand"
TASK = SCOUT / "build/SCOUT-TASK.md"
# (report directory, glob) for every helper that hands work back
SOURCES = [(SCOUT / "build/cand", "scout-*.md"),
           (WORKER / "build/cand", "worker-*.md"),
           (DELTA / "build/cand", "delta-*.md")]
ARCHIVE = pathlib.Path("build/cand/archive")


def load(path, name):
    import importlib.util
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


t10 = load("build/t10.py", "t10")
auto = load("build/auto.py", "auto")
fm = load("build/fastmatch.py", "fm")

CLAIM = re.compile(r"^MATCH\s+(\w+)\s+(0x[0-9a-fA-F]+)\s+(\d+)\s+(\w+)\s+(\S+)\s*$")


def body_of(rel, report=None):
    path = pathlib.Path(rel)
    if path.is_absolute():
        return body_at(path)
    roots = ([report.parent] if report is not None else []) + [SCOUT, WORKER, DELTA]
    for root in roots:
        candidate = root / rel
        if candidate.exists():
            return body_at(candidate)
    # Helpers write candidates to build/cand/ but often report them under src/<mod>/.
    # Fall back to the basename inside the report's own directory before rejecting.
    if report is not None:
        candidate = report.parent / pathlib.Path(rel).name
        if candidate.exists():
            return body_at(candidate)
    return None


def body_at(path):
    if not path.exists():
        return None
    lines = path.read_text().splitlines()
    return "\n".join(l for l in lines if not l.lstrip().startswith("#include")).strip()


def installed(mod, name):
    return (pathlib.Path(t10.srcdir_for(mod)) / f"{name}.c").exists()


PROSE = re.compile(r"^(\d+)\s*[-–—:]\s*`(0x[0-9a-fA-F]{7,8})`")


def claims(report):
    """`MATCH ...` lines, and the prose `84 — `0x020135fc`` form, in MATCH sections."""
    text = report.read_text()
    m = re.search(r"^#\s*scout\s+(\w+)", text, re.M)
    mod = m.group(1) if m else "arm9"
    auto.set_module(mod)
    section = ""
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("##"):
            parts = stripped.split()
            section = parts[1] if len(parts) > 1 else ""
            continue
        m = CLAIM.match(stripped)
        if m:
            yield m.group(1), int(m.group(2), 16), int(m.group(3)), m.group(4), m.group(5)
            continue
        if section.startswith("MATCH"):
            # a line may carry several pairs: "148 — `0x0200fe9c` and 156 — `0x020100b4`"
            for m in re.finditer(r"(\d+)\s*[–—:-]\s*`(0x[0-9a-fA-F]+)`", stripped):
                size, addr = int(m.group(1)), int(m.group(2), 16)
                name = auto.FUNCS.get(addr, (None, 0))[0]
                if name:
                    yield mod, addr, size, name, f"build/cand/{name}.c"


ADDR_RE = re.compile(r"`(0x[0-9a-fA-F]{7,8})`")
# A line that looks like it was meant to be a claim: "MATCH ..." or "84 — `0x...`".
CLAIMLIKE = re.compile(r"^(MATCH|\d+\s*[-–—:])")


def orphan_addresses(report):
    """Addresses named in a MATCH section that claims() did not turn into a claim."""
    try:
        text = report.read_text(encoding="utf-8")
    except (FileNotFoundError, OSError):
        return []  # the helper rewrote or replaced it mid-run; nothing to check
    section, found = "", []
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("##"):
            parts = stripped.split()
            section = parts[1] if len(parts) > 1 else ""
            continue
        if not section.startswith("MATCH"):
            continue
        if CLAIM.match(stripped) or PROSE.match(stripped):
            continue
        if not CLAIMLIKE.match(stripped):
            continue
        found.extend(ADDR_RE.findall(stripped))
    return found


def archive(report):
    """Move a processed report aside, never overwriting (and never raising)."""
    ARCHIVE.mkdir(parents=True, exist_ok=True)
    dest = ARCHIVE / report.name
    n = 2
    while dest.exists():
        dest = ARCHIVE / f"{report.name[:-3]}.{n}.md"
        n += 1
    shutil.move(str(report), str(dest))


def batch_install(claims):
    """Install every claim, then relink once, then hash once.

    Returns False (tree restored) if the batch digest fails, so the caller can
    fall back to per-claim try_unit and find the offender.
    """
    snap, created = {}, []
    try:
        for mod in {c[0] for c in claims}:
            dl = pathlib.Path(t10.delinks_for(mod))
            srcdir = pathlib.Path(t10.srcdir_for(mod))
            srcdir.mkdir(parents=True, exist_ok=True)
            text = snap[mod] = dl.read_text(encoding="utf-8", errors="replace")
            for m, name, addr, size, body in claims:
                if m != mod:
                    continue
                src = srcdir / (name + ".c")
                if not src.exists():
                    created.append(src)
                src.write_text('#include "types.h"' + '\n\n' + body, encoding='utf-8')
                if t10._entry_span(text.splitlines(keepends=True), src.as_posix()) is None:
                    text = text.rstrip() + "\n\n" + t10.entry_for(
                        srcdir.as_posix(), name, addr, size)
            dl.write_text(text, encoding="utf-8")
        ok, _log = t10.configure_build()
        if ok and t10.sha1():
            return True
    except BaseException:
        pass
    for mod, text in snap.items():
        pathlib.Path(t10.delinks_for(mod)).write_text(text, encoding="utf-8")
    for src in created:
        src.unlink(missing_ok=True)
    t10.reconfigure()
    return False


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--install", action="store_true", help="install verified claims")
    ap.add_argument("--batch", action="store_true",
                    help="install every claim, relink and hash once instead of per unit")
    ap.add_argument("--next", metavar="TEXT", help="rewrite the scout's band and reset its tree")
    ap.add_argument("--pull", action="store_true", help="sync the scout's tree to main, keep the band")
    ap.add_argument("--force", action="store_true", help="allow --next with an unprocessed report")
    args = ap.parse_args()

    if args.pull:
        for tree in (SCOUT, WORKER, DELTA):
            if not tree.exists():
                continue
            subprocess.run(["git", "-C", str(tree), "reset", "--hard", "main"],
                           capture_output=True, text=True)
            head = subprocess.run(["git", "-C", str(tree), "log", "-1", "--oneline"],
                                  capture_output=True, text=True).stdout.strip()
            print(f"{tree.name} tree: {head or '(unavailable)'}")
        return 0

    if args.next:
        pending = [p for directory, pattern in SOURCES
                   for p in sorted(directory.glob(pattern))]
        if pending and not args.force:
            print("refusing --next: unprocessed report(s): "
                  + ", ".join(p.name for p in pending))
            print("run inbox.py (and --install), or pass --force")
            return 1
        text = TASK.read_text()
        text = re.sub(r"^Band:.*$", f"Band: {args.next}", text, count=1, flags=re.M)
        TASK.write_text(text)
        done = subprocess.run(["git", "-C", str(SCOUT), "reset", "--hard", "main"],
                              capture_output=True, text=True)
        print(done.stdout.strip() or done.stderr.strip())
        print(f"scout band: {args.next}")

    reports = []
    for directory, pattern in SOURCES:
        reports.extend(sorted(p for p in directory.glob(pattern) if not p.name.endswith(".tmp")))
    if not reports:
        print("no scout reports pending")
        return 0

    ok = bad = skipped = 0
    todo = []          # verified claims waiting for a single batch relink
    for report in reports:
        print(report.name)
        for mod, addr, size, name, rel in claims(report):
            label = f"{mod} {addr:#010x} {size:3d} {name}"
            if installed(mod, name):
                print(f"  skip  {label} (already installed)")
                skipped += 1
                continue
            body = body_of(rel, report)
            if not body:
                print(f"  miss  {label} (no candidate at {rel})")
                bad += 1
                continue
            if not fm.compare(body, mod, addr, size):
                print(f"  fail  {label} (not byte-identical here)")
                bad += 1
                continue
            if not args.install:
                print(f"  ok    {label} (verified; --install to take it)")
                ok += 1
                continue
            if args.batch and args.install:
                todo.append((mod, name, addr, size, body))
                print(f"  ok    {label} (staged)")
                continue
            if t10.try_unit(name, addr, size, body, mod=mod):
                print(f"  ok    {label} (installed)")
                ok += 1
            else:
                print(f"  fail  {label} (ROM digest rejected it)")
                bad += 1
        if args.install:
            archive(report)

    if todo:
        if batch_install(todo):
            print(f"  batch  all {len(todo)} staged unit(s) linked; ROM digest OK")
            ok += len(todo)
        else:
            print(f"  batch  digest failed for the set; testing one at a time")
            for mod, name, addr, size, body in todo:
                if t10.try_unit(name, addr, size, body, mod=mod):
                    print(f"  ok    {mod} {addr:#010x} {size:3d} {name} (installed)")
                    ok += 1
                else:
                    print(f"  fail  {mod} {addr:#010x} {size:3d} {name} (ROM digest rejected it)")
                    bad += 1

    for report in reports:
        lost = orphan_addresses(report)
        if lost:
            print(f"  WARN {report.name}: {len(lost)} address(es) in a MATCH section "
                  f"were not parsed: {', '.join(lost[:6])}")

    print(f"{ok} verified, {bad} rejected, {skipped} already installed")
    return 1 if bad else 0


if __name__ == "__main__":
    raise SystemExit(main())
