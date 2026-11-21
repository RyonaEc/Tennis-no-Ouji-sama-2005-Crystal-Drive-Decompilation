"""List unmatched functions in a size band, grouped by mnemonic skeleton.

Families come first: one source shape covers every member, and the constants
that differ between them are exactly the ones a substitution script can swap.

Usage: python tools/match/band.py arm9 0x50 0x54
"""
import collections
import importlib.util
import pathlib
import sys


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


auto = load("tools/match/auto.py", "auto")
fm = load("tools/match/fastmatch.py", "fm")

mod = sys.argv[1]
sizes = [int(s, 0) for s in sys.argv[2:]] or [0x50, 0x54]
auto.set_module(mod)
src = pathlib.Path(f"src/{mod}")
have = {p.stem for p in src.glob("*.c")}

loose = []
groups = collections.defaultdict(list)
for addr, (name, size) in sorted(auto.FUNCS.items()):
    if name in have or size not in sizes:
        continue
    try:
        ins = fm.parse_ins(addr, size)
    except Exception:
        continue
    if not ins:
        continue
    loose.append((addr, name, size))
    groups[" ".join(i.mnemonic for i in ins)].append((addr, name, size))

print(f"{mod}: {len(loose)} unmatched at sizes {[hex(s) for s in sizes]}")
print(f"{len(groups)} distinct skeletons, "
      f"{sum(1 for g in groups.values() if len(g) > 1)} with siblings")
for sig, members in sorted(groups.items(), key=lambda kv: (-len(kv[1]), kv[1][0][0])):
    if len(members) < 2:
        continue
    print(f"\n[{len(members)}] {members[0][2]}B  {sig}")
    for addr, name, size in members:
        print(f"     {addr:#010x} {name}")
