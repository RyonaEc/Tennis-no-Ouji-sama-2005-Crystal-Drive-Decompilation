"""Sweep sub-0x10-byte stubs (skipped by autorun's size floor), sha1-gated."""
import importlib.util, pathlib, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('build/auto.py', 'auto')
t10 = load('build/t10.py', 't10')

TRIED = pathlib.Path('build/tried.txt')
tried = set()
if TRIED.exists():
    for line in TRIED.read_text(encoding='utf-8').splitlines():
        if '\t' in line:
            tried.add(tuple(line.split('\t', 1)))

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 40
mods = sys.argv[2:] or ['arm9', 'ov002', 'ov003', 'ov004', 'ov005', 'ov008', 'ov009']

total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {q.stem for q in srcdir.glob('*.c')} if srcdir.exists() else set()
    cands = []
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size >= 0x10 or (mod, name) in tried:
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
    print(f'[{mod}] tiny stubs: {len(cands)} untried, testing {n}', flush=True)
    hit = 0
    for size, name, addr, src in cands[:n]:
        got = t10.try_unit(name, addr, size, src, mod=mod)
        with TRIED.open('a', encoding='utf-8') as f:
            f.write(f'{mod}\t{name}\n')
        if got:
            hit += 1
    print(f'[{mod}] tiny matches: {hit}/{n}', flush=True)
    total += hit

print(f'total tiny matches: {total}', flush=True)
