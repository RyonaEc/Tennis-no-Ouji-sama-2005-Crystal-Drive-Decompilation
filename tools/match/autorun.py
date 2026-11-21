"""Auto-generate C for every eligible unmatched function and test it.

Usage: python tools/match/autorun.py [limit] [max_size] [module ...]
Modules: arm9 (default), or one or more of itcm, dtcm, ov000..ov010.
Prints MATCH/miss lines; matched units are kept in src/ and their delinks.

Candidates already tested are remembered in tools/match/tried.txt, so re-running with
a larger limit continues where the last run stopped instead of retesting the
same functions.
"""
import importlib.util
import pathlib
import sys

TRIED = pathlib.Path('tools/match/tried.txt')


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


def load_tried():
    if not TRIED.exists():
        return set()
    rows = set()
    for line in TRIED.read_text(encoding='utf-8').splitlines():
        if '\t' in line:
            mod, name = line.split('\t', 1)
            rows.add((mod, name))
    return rows


def record_tried(mod, name):
    with TRIED.open('a', encoding='utf-8') as f:
        f.write(mod + '\t' + name + '\n')


a = load('tools/match/auto.py', 'auto')
t10 = load('tools/match/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 40
max_size = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x60
modules = sys.argv[3:] or ['arm9']

total_hit = 0
for mod in modules:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {p.stem for p in srcdir.glob('*.c')}
    tried = load_tried()
    cands, bailed = [], 0
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size > max_size or size < 0x4 or (mod, name) in tried:
            continue
        try:
            fn = a.gen(addr)
            if not fn:
                continue
            src = a.emit(fn)
        except a.Bail:
            bailed += 1
            continue
        except Exception:
            bailed += 1
            continue
        cands.append((size, name, addr, size, src))

    cands.sort()
    n = min(limit, len(cands))
    print(f'[{mod}] candidates {len(cands)}  bailed {bailed}  '
          f'already tried {len([t for t in tried if t[0] == mod])}  testing {n}')
    hit = 0
    for size, name, addr, sz, src in cands[:n]:
        got = t10.try_unit(name, addr, sz, src, mod=mod)
        record_tried(mod, name)
        if got:
            hit += 1
    total_hit += hit
    print(f'[{mod}] generated matches: {hit}/{n}')

if len(modules) > 1:
    print(f'total generated matches: {total_hit}')
