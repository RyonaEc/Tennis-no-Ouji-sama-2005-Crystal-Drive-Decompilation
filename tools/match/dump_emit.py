"""Dump every generatable function's emitted C, one hash per line, for A/B diffs."""
import hashlib, importlib.util, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('tools/match/auto.py', 'auto')
mods = ['arm9'] + [f'ov{n:03d}' for n in range(11)]
for mod in mods:
    a.set_module(mod)
    for addr, (name, size) in sorted(a.FUNCS.items()):
        try:
            fn = a.gen(addr)
            text = a.emit(fn) if fn else ''
        except Exception as e:
            text = f'BAIL {e}'
        print(f'{mod} {name} {hashlib.sha1(text.encode()).hexdigest()[:16]}')
