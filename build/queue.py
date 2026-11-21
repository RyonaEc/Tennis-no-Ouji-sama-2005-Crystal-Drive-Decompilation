"""Hand a helper the next size band to work, and remember the ones already done.

A band is one function size in one module. Bands are consumed smallest-first
because small functions are the cheapest, and the state file means a helper that
restarts never repeats a band it already exhausted.

Usage:
    python build/queue.py ov002            # next band: "0x20 12"
    python build/queue.py ov002 --mark 0x20
"""
import argparse
import importlib.util
import pathlib
import re


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


auto = load("build/auto.py", "auto")
STATE = pathlib.Path("build/queue-state.txt")


def state():
    if not STATE.exists():
        return set()
    return {tuple(line.split()) for line in STATE.read_text().splitlines() if line.strip()}


def remaining(mod):
    auto.set_module(mod)
    have = {p.stem for p in pathlib.Path(f"src/{mod}").glob("*.c")}
    # skip what C cannot produce: Thumb `svc` stubs and recorded walls
    skip_names, skip_addrs = set(), set()
    for cand in (f"config/ATOJ/{mod}/symbols.txt",
                 f"config/ATOJ/arm9/{mod}/symbols.txt",
                 f"config/ATOJ/arm9/overlays/{mod}/symbols.txt"):
        path = pathlib.Path(cand)
        if path.exists():
            for line in path.read_text(encoding="utf-8").splitlines():
                m = re.match(r"(\S+)\s+kind:function\(thumb", line)
                if m:
                    skip_names.add(m.group(1))
            break
    wpath = pathlib.Path("build/walls.txt")
    if wpath.exists():
        for line in wpath.read_text(encoding="utf-8").splitlines():
            parts = line.split()
            if len(parts) >= 2 and parts[0] == mod:
                skip_addrs.add(int(parts[1], 16))
    counts = {}
    for addr, (name, size) in auto.FUNCS.items():
        if name in have or name in skip_names or addr in skip_addrs:
            continue
        counts[size] = counts.get(size, 0) + 1
    return counts


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mod")
    ap.add_argument("--mark", help="record this size as worked")
    args = ap.parse_args()

    done = state()
    if args.mark:
        done.add((args.mod, args.mark.lower()))
        STATE.write_text("".join(f"{m} {s}\n" for m, s in sorted(done)))
        print(f"marked {args.mod} {args.mark}")

    counts = remaining(args.mod)
    pending = sorted((size, n) for size, n in counts.items()
                     if (args.mod, hex(size)) not in done)
    if not pending:
        print("")
        return 0
    size, count = pending[0]
    print(f"{hex(size)} {count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
