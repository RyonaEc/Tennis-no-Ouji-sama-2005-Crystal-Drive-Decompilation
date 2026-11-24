"""Grind the queue unattended: re-spell every unit's best candidate, journal it.

One `respell.py` run costs a tool call and stops when the agent's turn ends.
This is the same search without the turn: it walks the parked walls first (their
best shapes are already written down, so they are the cheapest targets), then
the live unmatched queue smallest-first, re-spelling each unit's closest stored
candidate along the axes that decide register colouring, and it keeps going for
as long as it is left running.

The gate does not move. A variant that scores zero differing slots is handed to
`t10.try_unit`, which byte-compares it again and only keeps it if the linked ROM
still hashes to retail. Wrong variants are rejected by `fm.diff_summary` for
~0.1 s each and never earn a relink, so an overnight run is a few hundred
thousand scored variants instead of a few hundred.

Resumable: every finished unit is appended to `build/grind-journal.jsonl`, so a
restart, a crash or a kill skips what was already exhausted. The journal also
records each unit's best residue, which is what a later hand pass should start
from.

Usage:
    python tools/match/grind.py [--lane walls|queue|all] [--module M]...
                                [--hours H] [--limit N] [--budget N]
                                [--bases N] [--no-pad] [--dry-run] [--reset]

    --lane     walls (default), queue, or all (walls first, queue after)
    --module   restrict to a module; repeatable
    --hours    stop after this many hours (default 0: until the queue is empty)
    --limit    visit at most N units this run (default 0: no limit)
    --budget   variants scored per unit (default 600)
    --bases    stored candidates re-spelled per unit (default 3)
    --no-pad   do not add the dead-local variants (respell's --pad)
    --dry-run  list the units and their candidate sources, score nothing
    --reset    delete the journal first

One instance per build tree: `t10.try_unit` runs `configure.py` and `ninja rom`,
which are single-tree operations. Parallel workers belong in their own
worktrees, one module each. This tool never touches git.
"""
import importlib.util
import json
import pathlib
import re
import sys
import time


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
t10 = load("tools/match/t10.py", "t10")
respell = load("tools/match/respell.py", "respell")
fresh = load("tools/match/fresh.py", "fresh")
clone = load("tools/match/clone.py", "clone")

JOURNAL = pathlib.Path("build/grind-journal.jsonl")
WALLS = pathlib.Path("tools/match/walls.txt")
INCLUDE = re.compile(r'^\s*#include\s+"types\.h"\s*\n', re.M)
HEX8 = re.compile(r"[0-9a-fA-F]{8}")
WALL_RE = re.compile(r"^(\w+)\s+0x0*([0-9a-fA-F]+)\s+(\d+)B\s+(\S+)")

# Candidate files live under one flat directory and are named either for the
# function (`func_ov008_0204c810.c`) or for the bare address (`0204c810_x.c`),
# so the address is the reliable key.  Every overlay loads at the same base, so
# a bare address is ambiguous across modules: a name that mentions a module is
# indexed under that module and the lookup prefers the module-specific entry.
INDEX = {}


def strip_include(text):
    return INCLUDE.sub("", text, count=1)


def candidate_index():
    if INDEX:
        return INDEX
    root = pathlib.Path("build/cand")
    if not root.exists():
        return INDEX
    for path in root.iterdir():
        if not path.is_file() or path.suffix != ".c":
            continue
        mod = re.search(r"(ov\d{3})", path.stem)
        for hexed in set(HEX8.findall(path.stem)):
            key = int(hexed, 16)
            INDEX.setdefault((mod.group(1) if mod else None, key), []).append(path)
    return INDEX


def stored_candidates(mod, addr):
    index = candidate_index()
    paths = list(index.get((mod, addr), []))
    paths += [p for p in index.get((None, addr), []) if p not in paths]
    return paths


CLONE_INDEX = {}


def clone_candidates(mod, addr, size, cap):
    """Clone matched siblings: the shape most unmatched units have nowhere else.

    Most of the queue never had a `build/cand` file, so re-spelling alone would
    reach under a third of it.  A unit whose instruction skeleton equals an
    already-matched unit's is the cheapest kind of new candidate, and
    `clone.py` builds it without weakening anything: the constants are mapped
    by program order and refused on a clash.
    """
    if mod not in CLONE_INDEX:
        try:
            CLONE_INDEX[mod] = clone.index_for(mod)
        except Exception:
            CLONE_INDEX[mod] = {}
    try:
        found = clone.siblings(mod, addr, size, CLONE_INDEX[mod])
    except Exception:
        return []
    return [("clone:" + sibling, text) for sibling, text in found[:cap]]


def gen_candidate(mod, addr, size):
    """Fall back to the code generator when nothing stored compiles."""
    auto = fm.auto
    try:
        auto.set_module(mod)
        fn = auto.gen(addr, size)
        if not fn:
            return []
        return [("auto", auto.emit(fn))]
    except Exception:
        return []


def score(mod, addr, size, rows, dry):
    """Keep the candidates that compile, with their differing-slot count."""
    scored = []
    for label, text in rows:
        if "{" not in text:
            continue
        if dry:
            scored.append((0, label, text))
            continue
        try:
            _ok, n, diffs = fm.diff_summary(mod, addr, size, strip_include(text))
        except Exception:
            continue
        if n:
            scored.append((len(diffs), label, text))
    return scored


def bases_for(mod, addr, size, cap, dry):
    """The closest candidates for a unit, best first.

    Sources in order of how much they already know: a stored candidate from a
    previous hand pass, then a clone of a matched sibling with the same
    skeleton, then the code generator's attempt.
    """
    stored = []
    for path in stored_candidates(mod, addr):
        try:
            stored.append((path.name, path.read_text(encoding="utf-8", errors="replace")))
        except OSError:
            continue
    scored = score(mod, addr, size, stored, dry)
    if not scored:
        scored = score(mod, addr, size, clone_candidates(mod, addr, size, cap), dry)
    if not scored:
        scored = score(mod, addr, size, gen_candidate(mod, addr, size), dry)
    scored.sort(key=lambda row: row[0])
    return scored[:cap]


def grind_unit(mod, addr, size, name, budget, bases_cap, pad, dry, journal):
    """Spell one unit's candidates.  Returns (status, variants, best)."""
    bases = bases_for(mod, addr, size, bases_cap, dry)
    src_path = pathlib.Path("src", mod, name + ".c")
    if dry:
        label = ", ".join(b[1] for b in bases) or "no candidate"
        print("[grind] %-6s %#010x %5dB %-24s %s" % (mod, addr, size, name, label),
              flush=True)
        return "dry", 0, None
    if not bases:
        note(journal, dict(t="unit", mod=mod, addr=addr, name=name,
                           status="no-candidate", variants=0, best=None))
        print("[grind] %-6s %#010x %5dB %-24s no candidate" % (mod, addr, size, name),
              flush=True)
        return "no-candidate", 0, None

    tried, best, hit = 0, None, None
    for _score, base_label, text in bases:
        try:
            variants = respell.variants(text, pad=pad)
        except Exception:
            continue
        for vlabel, source in variants:
            if tried >= budget:
                break
            body = strip_include(source)
            try:
                ok, n, diffs = fm.diff_summary(mod, addr, size, body)
            except Exception:
                continue
            tried += 1
            if not n:
                continue
            label = base_label + "+" + vlabel
            if best is None or len(diffs) < best[0]:
                best = (len(diffs), label)
            if ok:
                out = pathlib.Path("build/cand/grind_%s.c" % name)
                out.write_text(source)
                kept = t10.try_unit(name, addr, size, body, mod=mod)
                note(journal, dict(t="match", mod=mod, addr=addr, name=name,
                                   label=label, kept=bool(kept), candidate=out.as_posix()))
                if kept:
                    print("[grind] MATCH  %-6s %#010x %5dB %-24s %s"
                          % (mod, addr, size, name, label), flush=True)
                else:
                    print("[grind] bytes  %-6s %#010x %5dB %-24s %s "
                          "-> rejected by the digest, continuing"
                          % (mod, addr, size, name, label), flush=True)
                hit = label
                break
        if hit or tried >= budget:
            break

    status = "matched" if hit else "exhausted"
    note(journal, dict(t="unit", mod=mod, addr=addr, name=name, status=status,
                       variants=tried, best=(best[1] if best else None),
                       slots=(best[0] if best else None)))
    if not hit:
        where = "%d slots %s" % best if best else "no compiling variant"
        print("[grind] %-6s %#010x %5dB %-24s %d variants, best %s"
              % (mod, addr, size, name, tried, where), flush=True)
    return status, tried, best


def note(journal, record):
    with journal.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(record) + "\n")


def done_units(journal):
    """Units a previous run finished, matched or exhausted."""
    if not journal.exists():
        return set()
    out = set()
    for line in journal.read_text(encoding="utf-8", errors="replace").splitlines():
        try:
            record = json.loads(line)
        except ValueError:
            continue
        if record.get("t") in ("unit", "match"):
            out.add((record.get("mod"), record.get("addr")))
    return out


def wall_units(modules):
    if not WALLS.exists():
        return []
    out = []
    for line in WALLS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = WALL_RE.match(line)
        if not m:
            continue
        mod, addr, size, name = m.group(1), int(m.group(2), 16), int(m.group(3)), m.group(4)
        if modules and mod not in modules:
            continue
        out.append((size, mod, addr, size, name))
    return out


def queue_units(modules, lo, hi):
    mods = modules or fresh.modules()
    out = []
    for mod in mods:
        try:
            rows = fresh.unmatched(mod)
        except Exception:
            continue
        for _m, addr, size, name in rows:
            if lo <= size <= hi:
                out.append((size, mod, addr, size, name))
    return out


def main(argv):
    if "-h" in argv or "--help" in argv:
        print(__doc__.strip())
        return 0
    lane, hours, limit, budget, bases_cap = "walls", 0.0, 0, 600, 3
    lo, hi = 4, 0x200
    pad, dry, reset, modules = True, False, False, []
    for i, a in enumerate(argv):
        if a == "--lane":
            lane = argv[i + 1]
        elif a == "--module":
            modules.append(argv[i + 1])
        elif a == "--hours":
            hours = float(argv[i + 1])
        elif a == "--limit":
            limit = int(argv[i + 1], 0)
        elif a == "--budget":
            budget = int(argv[i + 1], 0)
        elif a == "--bases":
            bases_cap = int(argv[i + 1], 0)
        elif a == "--min":
            lo = int(argv[i + 1], 0)
        elif a == "--max":
            hi = int(argv[i + 1], 0)
        elif a == "--no-pad":
            pad = False
        elif a == "--dry-run":
            dry = True
        elif a == "--reset":
            reset = True

    journal = JOURNAL
    journal.parent.mkdir(parents=True, exist_ok=True)
    if reset and journal.exists():
        journal.unlink()
    finished = set() if dry else done_units(journal)

    # The walls lane stays in front: those units have a written-down best shape,
    # so they are the cheapest ground.  Each lane is ordered smallest-first.
    units = []
    if lane in ("walls", "all"):
        units += sorted(wall_units(modules), key=lambda row: (row[0], row[1], row[2]))
    if lane in ("queue", "all"):
        units += sorted(queue_units(modules, lo, hi), key=lambda row: (row[0], row[1], row[2]))
    seen = set()
    todo = []
    for size, mod, addr, sz, name in units:
        if (mod, addr) in seen:
            continue
        seen.add((mod, addr))
        if pathlib.Path("src", mod, name + ".c").exists():
            continue
        if (mod, addr) in finished:
            continue
        todo.append((size, mod, addr, sz, name))

    print("[grind] lane=%s units=%d already-done=%d budget=%d/unit pad=%s%s"
          % (lane, len(todo), len(finished), budget, pad, " dry-run" if dry else ""),
          flush=True)
    if not dry:
        note(journal, dict(t="start", lane=lane, units=len(todo), budget=budget,
                           pad=pad, modules=modules or None,
                           when=time.strftime("%Y-%m-%dT%H:%M:%S")))

    started = time.monotonic()
    counts = {"matched": 0, "exhausted": 0, "no-candidate": 0, "dry": 0}
    variants = visited = 0
    reason = "queue-empty"
    for size, mod, addr, sz, name in todo:
        if limit and visited >= limit:
            reason = "limit"
            break
        if hours and time.monotonic() - started >= hours * 3600:
            reason = "hours"
            break
        if pathlib.Path("src", mod, name + ".c").exists():
            continue
        visited += 1
        try:
            status, tried, _best = grind_unit(mod, addr, sz, name, budget,
                                              bases_cap, pad, dry, journal)
        except KeyboardInterrupt:
            reason = "interrupted"
            break
        except Exception as exc:
            print("[grind] %-6s %#010x %-24s error: %s: %s"
                  % (mod, addr, name, type(exc).__name__, exc), flush=True)
            continue
        counts[status] = counts.get(status, 0) + 1
        variants += tried

    elapsed = time.monotonic() - started
    print("[grind] stop reason=%s units=%d matched=%d exhausted=%d "
          "no-candidate=%d variants=%d in %.0fs"
          % (reason, visited, counts["matched"], counts["exhausted"],
             counts["no-candidate"], variants, elapsed), flush=True)
    if not dry:
        note(journal, dict(t="stop", reason=reason, units=visited,
                           matched=counts["matched"], variants=variants,
                           seconds=round(elapsed)))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
