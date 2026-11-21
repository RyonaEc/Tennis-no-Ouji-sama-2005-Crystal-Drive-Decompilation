"""Declarations must precede statements.

The hoist initialisers were spliced in at index 1, which pushed the call-temp
declaration (`u32 t0, t1, ...;`) below them, so the function began with a
statement followed by a declaration.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old = """    if fn['calls']:
        decls = ', '.join(f't{i}' for i in range(len(fn['calls'])))
        body.insert(0, f'    u32 {decls};')
    if hoist_names:
        body.insert(0, '    u32 ' + ', '.join(hoist_names) + ';')
        body[1:1] = hoist_assigns
"""
new = """    decls = []
    if fn['calls']:
        decls.append('    u32 ' + ', '.join(f't{i}' for i in range(len(fn['calls']))) + ';')
    if hoist_names:
        decls.append('    u32 ' + ', '.join(hoist_names) + ';')
    if decls:
        body[0:0] = decls + hoist_assigns
"""
assert t.count(old) == 1, t.count(old)
P.write_text(t.replace(old, new, 1), encoding="utf-8")
print("declaration order fixed")
