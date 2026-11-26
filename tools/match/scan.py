"""Rank untried functions by how likely this mwcc build can reproduce them.

Usage:
    python tools/match/scan.py [--show N] [--min B] [--max B] [--limit N] [--parked]
                               mod [mod ...]

Units already recorded in `walls.txt` are skipped by default - their residue is
known and re-deriving them moves no counter. `--parked` ranks them anyway.

Functions are scored on codegen shapes that have never matched: pre-indexed
writeback, offsets too large for imm12, materialised field addresses, mla and
multi-register transfers. Lower is better; 0 means nothing known is in the way.
"""

import importlib.util
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SPEC = importlib.util.spec_from_file_location(
    "fm", os.path.join(ROOT, "tools", "match", "fastmatch.py")
)
fm = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(fm)


def parked(mod):
    """Addresses `walls.txt` has already parked for this module.

    A parked unit has had its residue measured and written down; ranking it again
    is the same pass twice. `--parked` puts them back in the list.
    """
    out = set()
    path = os.path.join(ROOT, "tools", "match", "walls.txt")
    try:
        text = open(path, encoding="utf-8", errors="replace").read()
    except OSError:
        return out
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split()
        if len(parts) >= 2 and parts[0] == mod:
            try:
                out.add(int(parts[1], 16))
            except ValueError:
                pass
    return out


def load(mod, lo, hi, limit, skip=()):
    fm.auto.set_module(mod)
    done = {f[:-2] for f in os.listdir(os.path.join(ROOT, "src", mod))}
    rows = sorted(
        (sz, a, n) for a, (n, sz) in fm.auto.FUNCS.items()
        if n not in done and a not in skip
    )
    pick = [r for r in rows if lo <= r[0] <= hi][:limit]
    if not pick:
        return {}
    out = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "match", "udis.py"), mod]
        + [hex(r[1]) for r in pick],
        capture_output=True,
        text=True,
        cwd=ROOT,
    ).stdout
    funcs, cur = {}, None
    for ln in out.splitlines():
        m = re.match(r"=== \S+ (0x[0-9a-f]+) (\S+) size (\d+)", ln)
        if m:
            cur = (mod, m.group(2), int(m.group(3)))
            funcs[cur] = []
            continue
        if cur and ln.strip():
            funcs[cur].append(ln.split(None, 1)[-1].strip())
    return funcs


def score(ins):
    total = 0
    for i, ins in enumerate(ins):
        if "]!" in ins:
            total += 3
        if re.search(r"#0x[0-9a-f]{4,}", ins):
            total += 1
        m = re.match(r"add\w* (r\d+|ip), (r\d+|ip), #0x[0-9a-f]+$", ins)
        if m:
            nxt = ins[i + 1] if i + 1 < len(ins) else ""
            if "[" in nxt and m.group(1) in nxt:
                total += 2
        if "mla" in ins or "smull" in ins:
            total += 1
        if "ldm" in ins or "stm" in ins:
            total += 1
        if "apsr" in ins or ins.startswith("msr") or ins.startswith("mrs"):
            total += 5
    return total


def main(argv):
    show, lo, hi, limit = 3, 60, 200, 70
    include_parked = False
    mods, rest = [], list(argv)
    while rest:
        a = rest.pop(0)
        if a == "--show":
            show = int(rest.pop(0))
        elif a == "--min":
            lo = int(rest.pop(0))
        elif a == "--max":
            hi = int(rest.pop(0))
        elif a == "--limit":
            limit = int(rest.pop(0))
        elif a == "--parked":
            include_parked = True
        else:
            mods.append(a)
    rank = []
    for mod in mods:
        skip = set() if include_parked else parked(mod)
        for (m, name, size), ins in load(mod, lo, hi, limit, skip).items():
            rank.append((score(ins), size, m, name, ins))
    rank.sort(key=lambda r: (r[0], r[1]))
    for s, size, m, name, ins in rank:
        print("%3d %6d %-6s %s" % (s, size, m, name))
    for s, size, m, name, ins in rank[:show]:
        print("\n=== %s %s size %d score %d" % (m, name, size, s))
        for i in ins:
            print("   ", i)


if __name__ == "__main__":
    main(sys.argv[1:])
