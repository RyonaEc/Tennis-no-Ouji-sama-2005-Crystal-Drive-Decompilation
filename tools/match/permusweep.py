"""Triage every parked candidate, then permute the closest ones.

`permute.py` is good at one function at a time and knows nothing about the rest
of the queue, so the order candidates are tried in is whatever the operator
remembers.  Two things are missing, both identified by how other projects work:

* **Triage.** decomp-triage's lesson is that failures are not one kind: a
  candidate that is instruction-identical except for register choice is a
  permuter problem, while a structurally different one is a source problem the
  permuter cannot fix, and sifting them costs more than the search itself.  The
  aligned weighted score in `permute.py` already separates the two
  (`structural` pairs vs weighted `cost`), so this tool scores every candidate
  once, cheaply, and sorts by it.
* **Scale.** The angle families and the 240-odd callers of
  `func_ov002_02057068` are one candidate each; running them one command at a
  time is why the walls sat still.

Work list
---------
Candidates are `build/cand/*.c` whose file name contains the address of a
symbol that is not matched yet.  Names are matched on the last 5 or 6 hex digits,
because the candidates here are named for the address in various ways
(`ov2_9d988_clean.c`, `f68c78_v2.c`, `ov2_8f38c_12.c`).  An address that matches
two candidates takes the newest file; an address that is ambiguous across two
symbols is skipped.  A manifest can override discovery:

    tools/match/permusweep.tsv     one `mod addr size file.c` line per item

What it does
------------
1. Scores every candidate once (one compile each) and prints the triage table.
2. Spends `--time-each` seconds on the closest `--count` of them, using
   `permute.py`'s declaration sweep and annealed search.
3. Writes the best source found to `build/cand/sweep_<func>.c`, journals each
   result to `build/permusweep.jsonl`, and with `--install` runs `t10.try_unit`,
   which keeps a unit only when the ROM digest still matches.

A zero score is not a match until `t10.try_unit` and the digest agree, exactly as
for `reg.py`.

Usage
-----
    python tools/match/permusweep.py [--mod M] [--count N] [--time-each S]
        [--minutes S] [--jobs N] [--sweep-max N] [--install] [--dry-run]
        [--manifest FILE]
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


perm = load("tools/match/permute.py", "perm")


def symbols():
    """(mod, name, addr, size) for every function symbol in the config tree."""
    out = []
    root = pathlib.Path("config/ATOJ/arm9")
    files = [("arm9", root / "symbols.txt")]
    files += [(d.name, d / "symbols.txt") for d in sorted((root / "overlays").glob("*"))]
    for mod, path in files:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        for m in re.finditer(r"(func_\w+) kind:function\([^,]*,size=0x([0-9a-fA-F]+)\)"
                             r" addr:0x([0-9a-fA-F]+)", text):
            out.append((mod, m.group(1), int(m.group(3), 16), int(m.group(2), 16)))
    return out


def matched(mod, name):
    return pathlib.Path("src", mod, name + ".c").exists()


def discover(mod_filter):
    """{address: [candidate files]} for unmatched functions.

    Address suffix lookup, not a per-file scan over every symbol: `build/cand`
    holds thousands of drafts and the config several thousand symbols, so the
    naive double loop costs minutes before the first line of output.

    Several files usually exist for one address (drafts, variants, a `best_`
    snapshot).  They are all kept and scored in `triage`, because picking the
    newest file is how a run ends up permuting a worse draft than the one sitting
    next to it.
    """
    table = symbols()
    index = {}
    for mod, name, addr, size in table:
        key = "%08x" % addr
        for k in {key, key[-6:], key[-5:]}:
            index.setdefault(k, []).append((mod, name, addr, size))
    work, ambiguous = {}, set()
    for path in sorted(pathlib.Path("build/cand").glob("*.c")):
        stem = path.stem.lower()
        hits = {}
        for token in set(re.findall(r"[0-9a-f]{5,8}", stem)):
            for entry in index.get(token, ()):
                hits[entry[2]] = entry
        if len(hits) != 1:
            if len(hits) > 1:
                ambiguous.add(path.name)
            continue
        mod, name, addr, size = next(iter(hits.values()))
        if mod_filter and mod != mod_filter:
            continue
        if matched(mod, name):
            continue
        entry = work.get(addr)
        if entry is None:
            work[addr] = [mod, name, addr, size, [path]]
        else:
            entry[4].append(path)
    return list(work.values()), ambiguous


def read_manifest(path):
    items = []
    for line in pathlib.Path(path).read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        if len(parts) < 4:
            continue
        items.append((parts[0], "", int(parts[1], 0), int(parts[2], 0), [pathlib.Path(parts[3])]))
    return items


def triage(items, jobs=1):
    """Score every candidate file once, in parallel, and keep the best per address.

    Scoring is one compile per file and there can be several hundred files, so
    this is the slowest phase of a sweep.  It is also embarrassingly parallel,
    which is why `permute.eval_batch` exists.
    """
    plan, results = [], []
    for mod, name, addr, size, paths in items:
        paths = paths if isinstance(paths, list) else [paths]
        paths = sorted(paths, key=lambda f: (0 if f.stem.startswith("best_") else 1,
                                             -f.stat().st_mtime))[:2]
        for path in paths:
            plan.append((mod, name, addr, size, path))
    jobs_in = [(m, a, s, [perm.body_of(p.read_text(encoding="utf-8", errors="ignore"))])
               for m, _, a, s, p in plan]
    if jobs > 1 and False:
        try:
            import concurrent.futures
            with concurrent.futures.ProcessPoolExecutor(max_workers=jobs) as pool:
                results = [r[0] for r in pool.map(perm.eval_batch, jobs_in)]
        except Exception:
            results = []
    if not results:
        results = []
        for n, job in enumerate(jobs_in, 1):
            results.append(perm.eval_batch(job)[0])
            if n % 25 == 0:
                print("  scored %d/%d" % (n, len(jobs_in)), flush=True)

    best = {}
    for (mod, name, addr, size, path), s in zip(plan, results):
        key = (mod, addr)
        cur = best.get(key)
        if cur is None or (s is not None and (cur[0] is None or s < cur[0])):
            best[key] = (s, mod, name, addr, size, path)
    rows = list(best.values())
    rows.sort(key=lambda r: (r[0] is None, r[0] if r[0] else (10 ** 6, 10 ** 6, 10 ** 6)))
    return rows


def main(argv):
    flags = [a for a in argv if a.startswith("--")]
    args = [a for a in argv if not a.startswith("--")]
    opt = {"--count": 8, "--time-each": 60, "--minutes": 20, "--jobs": 1,
           "--sweep-max": 6, "--iters": 120, "--islands": 3,
           "--min-structural": 0, "--max-structural": 10 ** 6}
    for flag in list(opt):
        if flag in argv:
            opt[flag] = int(argv[argv.index(flag) + 1])
    mod_filter = None
    if "--mod" in argv:
        mod_filter = argv[argv.index("--mod") + 1]
    install = "--install" in flags
    dry = "--dry-run" in flags
    manifest = None
    if "--manifest" in argv:
        manifest = argv[argv.index("--manifest") + 1]
    elif pathlib.Path("tools/match/permusweep.tsv").exists():
        manifest = "tools/match/permusweep.tsv"

    if manifest:
        items = read_manifest(manifest)
        print("manifest %s: %d candidates" % (manifest, len(items)))
    else:
        items, ambiguous = discover(mod_filter)
        print("discovered %d candidates%s"
              % (len(items), " (%d ambiguous file names skipped)" % len(ambiguous) if ambiguous else ""))
    if not items:
        return 2

    rows = triage(items, opt["--jobs"])
    # A structural difference means the source is still wrong; a pure register
    # difference means the compiler chose differently, which the search has been
    # demonstrated not to change.  `--min-structural 1` skips the second kind.
    lo, hi = opt["--min-structural"], opt["--max-structural"]
    rows = [r for r in rows if r[0] is None or lo <= r[0][0] <= hi]
    if not rows:
        print("no candidate in the structural range %d..%d" % (lo, hi))
        return 2
    print("\n%-34s %10s %7s  %s" % ("function", "structural", "cost", "candidate"))
    for s, mod, name, addr, size, path in rows[: max(opt["--count"], 12)]:
        if s is None:
            print("%-34s %10s %7s  %s" % (name, "no-compile", "-", path.name))
            continue
        note = ""
        if s[0] == 0:
            note = "  <- exact" if s[1] == 0 else "  <- regperm only, permuter gold"
        print("%-34s %10d %7d  %s%s" % (name, s[0], s[1], path.name, note))
    if dry:
        return 0

    deadline = time.time() + opt["--minutes"] * 60
    journal = pathlib.Path("build/permusweep.jsonl")
    done = 0
    for s, mod, name, addr, size, path in rows[: opt["--count"]]:
        if time.time() > deadline:
            print("time budget spent after %d functions" % done)
            break
        if s is not None and s[0] == 0:
            print("%s already exact; install it with reg.py" % name)
        print("\n== %s  %s  %dB  (%s)" % (name, mod, size, path.name))
        budget = min(opt["--time-each"], max(5, deadline - time.time()))
        argv2 = [mod, hex(addr), str(size), str(path),
                 "--sweep-max", str(opt["--sweep-max"]), "--iters", str(opt["--iters"]),
                 "--islands", str(opt["--islands"]), "--time", str(int(budget)),
                 "--jobs", str(opt["--jobs"]), "--seed", "1"]
        if install:
            argv2.append("--install")
        try:
            rc = perm.main(argv2)
        except SystemExit as exc:
            rc = exc.code
        result = {"name": name, "mod": mod, "addr": hex(addr), "size": size,
                  "candidate": path.name, "rc": rc, "at": time.strftime("%Y-%m-%dT%H:%M:%S")}
        with journal.open("a", encoding="utf-8") as fh:
            fh.write(json.dumps(result) + "\n")
        done += 1
        if rc == 0:
            print("** %s matched" % name)
        elif rc == 3:
            print("!! %s scored 0 but the ROM digest rejected it - the masked object "
                  "compare is not the gate; not a match" % name)
    print("\n%d functions searched this run, journal %s" % (done, journal))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
