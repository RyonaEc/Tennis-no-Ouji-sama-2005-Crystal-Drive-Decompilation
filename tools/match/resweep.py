"""Re-test every function the current generator can express, ignoring tried.txt.

The generator changed (conditional runs, stack slots, homed args), so earlier
misses are stale.  Only matches are kept; misses are not recorded.
"""
import importlib.util, pathlib, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('tools/match/auto.py', 'auto')
t10 = load('tools/match/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 100
max_size = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x80
mods = sys.argv[3:] or ['arm9']

total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {q.stem for q in srcdir.glob('*.c')} if srcdir.exists() else set()
    cands = []
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size > max_size or size < 0x4:
            continue
        try:
            fn = a.gen(addr)
            if not fn:
                continue
            src = a.emit(fn)
        except Exception:
            continue
        cands.append((size, name, addr, src))
    cands.sort()
    n = min(limit, len(cands))
    print(f'[{mod}] generatable untried: {len(cands)}, testing {n}', flush=True)
    hit = 0
    for size, name, addr, src in cands[:n]:
        if t10.try_unit(name, addr, size, src, mod=mod):
            hit += 1
            print(f'  MATCH {name}', flush=True)
    print(f'[{mod}] matches: {hit}/{n}', flush=True)
    total += hit
print(f'total matches: {total}', flush=True)
