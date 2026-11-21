"""Retest functions whose prologue homes r0-r3: such sources take 5+ parameters."""
import importlib.util, pathlib, re, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('tools/match/auto.py', 'auto')
t10 = load('tools/match/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 25
mods = sys.argv[2:] or ['arm9']

def variants_of(src, name, extra):
    out = []
    pat = re.compile(rf'(u32|void) {name}\(([^)]*)\)')
    for n in extra:
        def rep(m):
            args = m.group(2)
            add = ', '.join(f'u32 a{i}' for i in range(len(args.split(', ')) if args.strip() else 0, n))
            return f'{m.group(1)} {name}({args + (", " if args.strip() else "") + add})'
        out.append(pat.sub(rep, src, count=1))
    return out

total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {q.stem for q in srcdir.glob('*.c')} if srcdir.exists() else set()
    cands = []
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size < 0x4:
            continue
        try:
            fn = a.gen(addr)
            if not fn:
                continue
            if not (a._ARGHOME and a._ARGHOME[1]):
                continue
            src = a.emit(fn)
        except Exception:
            continue
        cands.append((size, name, addr, src, fn['nargs']))
    cands.sort()
    print(f'[{mod}] homed-prologue functions: {len(cands)}', flush=True)
    hit = 0
    for size, name, addr, src, nargs in cands[:limit]:
        vs = variants_of(src, name, [5, 6] if nargs <= 4 else [nargs + 1, nargs + 2])
        got = t10.try_unit(name, addr, size, src, variants=vs, mod=mod)
        if got:
            hit += 1
            print(f'  HIT {name}', flush=True)
    print(f'[{mod}] homed matches: {hit}/{min(limit, len(cands))}', flush=True)
    total += hit
print(f'total homed matches: {total}', flush=True)
