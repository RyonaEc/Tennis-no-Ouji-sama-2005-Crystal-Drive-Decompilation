"""List unmatched leaf functions in a size band, cheapest first.

A leaf has no `bl`/`blx`, so it needs no callee signatures and usually cracks
with one reading of the disassembly. Units already matched (a source under
`src/<module>/`) or parked (`tools/match/walls.txt`) are excluded, so the output
is always a live queue - unlike `true_fresh.txt`.

Usage:
    python tools/match/leafq.py [lo] [hi] [--dirty] [--module MOD]

`lo`/`hi` are decimal sizes and default to 96 and 400. `--dirty` also keeps
units that contain ldm/stm/mul/mla, which the walls log shows mwcc will not
reproduce from an ordinary source shape.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fastmatch as fm

MODULES = ["arm9"] + ["ov%03d" % i for i in range(0, 11)]


def symbols_path(mod):
    if mod == "arm9":
        return "config/ATOJ/arm9/symbols.txt"
    return "config/ATOJ/arm9/overlays/%s/symbols.txt" % mod


def parked():
    """Every address mentioned in walls.txt, with or without a 0x prefix."""
    addrs = set()
    with open("tools/match/walls.txt", encoding="utf-8") as f:
        for line in f:
            for m in re.finditer(r"(?:0x)?0[23][0-9a-fA-F]{6}", line):
                addrs.add(int(m.group(0), 16))
    return addrs


def unmatched(mod, lo, hi, skip):
    path = symbols_path(mod)
    if not os.path.exists(path):
        return
    with open(path, encoding="utf-8") as f:
        for line in f:
            m = re.match(
                r"(\S+) kind:function\([^)]*size=(0x[0-9a-fA-F]+)\).*addr:(0x[0-9a-fA-F]+)",
                line,
            )
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            if not lo <= size <= hi:
                continue
            if addr in skip or os.path.exists("src/%s/%s.c" % (mod, name)):
                continue
            try:
                fm.retail(mod, addr, size)
                ins = fm.parse_ins(addr, size)
            except Exception:
                continue
            if not ins:
                continue
            mnemonics = [i.mnemonic for i in ins]
            if "bl" in mnemonics or "blx" in mnemonics:
                continue
            dirty = sum(
                1
                for m2 in mnemonics
                if m2.startswith(("ldm", "stm")) or m2 in ("mul", "mla")
            )
            yield size, mod, addr, name, dirty


def main(argv):
    args = [a for a in argv if not a.startswith("--")]
    lo = int(args[0]) if len(args) > 0 else 96
    hi = int(args[1]) if len(args) > 1 else 400
    dirty_ok = "--dirty" in argv
    only = None
    for a in argv:
        if a.startswith("--module="):
            only = a.split("=", 1)[1]

    mods = [only] if only else MODULES
    skip = parked()
    rows = []
    for mod in mods:
        rows.extend(unmatched(mod, lo, hi, skip))
    if not dirty_ok:
        rows = [r for r in rows if r[4] == 0]
    rows.sort()
    print("%d leaf candidates %d-%dB%s" % (len(rows), lo, hi, "" if dirty_ok else ", clean"))
    for size, mod, addr, name, _ in rows:
        print("%4dB %-6s 0x%08x %s" % (size, mod, addr, name))


if __name__ == "__main__":
    main(sys.argv[1:])
