"""List the functions that are genuinely unmatched, derived from the live tree.

`true_fresh.txt` is a snapshot, and it has gone stale more than once: units that
already have a `src/` file still appear in it, so time gets spent writing C for
code that is already matched. This rebuilds the queue every run from two files
that cannot lie:

  * `symbols.txt` names every function and gives its address;
  * `delinks.txt` lists a `.text start/end` range for each unit, preceded by the
    `src/...` line when that unit has a source.

A function symbol that no source-backed range covers is still delinked
assembly - that is the queue.

Usage:
    python tools/match/fresh.py [--module ov009] [--min N] [--max N]
                                [--limit N] [--write]

`--write` regenerates `tools/match/true_fresh.txt` so the snapshot agrees with
the tree again.
"""
import pathlib
import re
import sys

SYMBOL = re.compile(r"^(\S+)\s+kind:(\S+?)\s+addr:0x([0-9a-fA-F]+)")
RANGE = re.compile(r"^\s*\.text\s+start:(0x[0-9a-fA-F]+)\s+end:(0x[0-9a-fA-F]+)")
SOURCE = re.compile(r"^(?:src|asm)/")


def modules():
    root = pathlib.Path("config/ATOJ/arm9")
    found = ["arm9"] if (root / "delinks.txt").exists() else []
    for path in sorted((root / "overlays").glob("*/delinks.txt")):
        found.append(path.parent.name)
    return found


def config_path(mod, name):
    if mod == "arm9":
        return pathlib.Path("config/ATOJ/arm9") / name
    return pathlib.Path("config/ATOJ/arm9/overlays") / mod / name


def functions(mod):
    """(addr, name) for every function symbol in the module."""
    out = []
    path = config_path(mod, "symbols.txt")
    if not path.exists():
        return out
    for line in path.read_text(errors="replace").splitlines():
        m = SYMBOL.match(line)
        if not m:
            continue
        name, kind = m.group(1), m.group(2)
        if "function" in kind or kind == "arm" or name.startswith("func_"):
            out.append((int(m.group(3), 16), name))
    return sorted(out)


def matched_ranges(mod):
    """The .text ranges that have a source behind them."""
    out = []
    path = config_path(mod, "delinks.txt")
    if not path.exists():
        return out
    source = None
    for line in path.read_text(errors="replace").splitlines():
        stripped = line.strip()
        if SOURCE.match(stripped):
            source = stripped
            continue
        m = RANGE.match(line)
        if m:
            if source is not None:
                out.append((int(m.group(1), 16), int(m.group(2), 16)))
            source = None
    return out


def symbol_sizes(mod):
    """addr -> size from the `kind:function(arm,size=0x...)` field."""
    out = {}
    path = config_path(mod, "symbols.txt")
    if not path.exists():
        return out
    for line in path.read_text(errors="replace").splitlines():
        m = SYMBOL.match(line)
        if not m:
            continue
        s = re.search(r"size=0x([0-9a-fA-F]+)", m.group(2))
        if s:
            out[int(m.group(3), 16)] = int(s.group(1), 16)
    return out


def unmatched(mod):
    spans = matched_ranges(mod)
    funs = functions(mod)
    # The last function in a module has no following symbol to bound it, so it
    # used to be reported as 4 bytes and swept against nothing - a 500-byte unit
    # was scored as a one-instruction stub.  The symbol's own size is the only
    # remaining evidence for its end.
    sizes = symbol_sizes(mod)
    rows = []
    for i, (addr, name) in enumerate(funs):
        if any(lo <= addr < hi for lo, hi in spans):
            continue
        end = funs[i + 1][0] if i + 1 < len(funs) else addr + (sizes.get(addr) or 4)
        size = max(end - addr, 4)
        if any(lo < addr + size and addr < hi for lo, hi in spans):
            # partly covered: the unit boundary disagrees, skip it rather than
            # report a size the delinks does not support
            size = max(size, 4)
        rows.append((mod, addr, size, name))
    return rows


def main(argv):
    mod, lo, hi, limit = None, 0, 1 << 30, 0
    write = "--write" in argv
    for i, a in enumerate(argv):
        if a == "--module":
            mod = argv[i + 1]
        elif a == "--min":
            lo = int(argv[i + 1], 0)
        elif a == "--max":
            hi = int(argv[i + 1], 0)
        elif a == "--limit":
            limit = int(argv[i + 1])

    rows = []
    for m in ([mod] if mod else modules()):
        rows.extend(unmatched(m))
    rows = [r for r in rows if lo <= r[2] <= hi]
    rows.sort(key=lambda r: (r[2], r[1]))

    if write:
        path = pathlib.Path("tools/match/true_fresh.txt")
        old = path.read_text().splitlines() if path.exists() else []
        path.write_text("\n".join("%s 0x%08x %d %s" % r for r in rows) + "\n")
        print("%d unmatched; snapshot had %d" % (len(rows), len(old)))

    for r in (rows[:limit] if limit else rows):
        print("%s 0x%08x %d %s" % r)
    if not write:
        print("%d unmatched function(s)" % len(rows))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
