#!/usr/bin/env python3
"""Rank the unmatched queue by how cheap a unit is to transcribe.

The filter is the one that produced the best hit rate in an outside trial on
this repo: small functions with at most one call, no ldm/stm, and no loops. The
score is a guess at effort, not a promise of a match - the digest still decides.

    python tools/match/pick.py ov002 --min 96 --max 420 --calls 1 --limit 20
    python tools/match/pick.py arm9 ov003 --loops 0 --symbolless

Output is one line per candidate, cheapest first:
    <module> <addr> <size>B bl=<n> branches=<n> ldm/stm=<n> <name>
"""
import argparse
import importlib.util
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("modules", nargs="+")
    ap.add_argument("--min", type=int, default=64)
    ap.add_argument("--max", type=int, default=420)
    ap.add_argument("--calls", type=int, default=1, help="max bl sites")
    ap.add_argument("--loops", type=int, default=0, help="max backward branches")
    ap.add_argument("--limit", type=int, default=25)
    args = ap.parse_args()

    fm = load(os.path.join(ROOT, "tools/match/fastmatch.py"), "fm")
    fresh = load(os.path.join(ROOT, "tools/match/fresh.py"), "fresh")

    rows = []
    for mod in args.modules:
        for _mod, addr, size, name in fresh.unmatched(mod):
            if not (args.min <= size <= args.max):
                continue
            try:
                ins = fm.parse_ins(addr, size)
            except Exception:
                continue
            bl = 0
            branches = 0
            multi = 0
            for i in ins:
                m = i.mnemonic
                if m == "bl":
                    bl += 1
                elif m.startswith("b") and m not in ("bic", "bics", "bfi", "bfc"):
                    branches += 1
                elif m in ("ldm", "ldmia", "ldmdb", "ldmib", "stm", "stmia", "stmdb", "stmib"):
                    multi += 1
            loops = 0
            for i in ins:
                if i.mnemonic.startswith("b") and i.op_str.startswith("#"):
                    try:
                        target = int(i.op_str[1:], 0)
                    except ValueError:
                        continue
                    if target < i.address:
                        loops += 1
            if bl > args.calls or loops > args.loops:
                continue
            rows.append((size, mod, addr, name, bl, branches, multi))

    rows.sort()
    for size, mod, addr, name, bl, branches, multi in rows[: args.limit]:
        print("%-6s 0x%08x %4dB bl=%d branches=%d ldstm=%d %s"
              % (mod, addr, size, bl, branches, multi, name))
    print("%d candidates" % len(rows), file=sys.stderr)


if __name__ == "__main__":
    main()
