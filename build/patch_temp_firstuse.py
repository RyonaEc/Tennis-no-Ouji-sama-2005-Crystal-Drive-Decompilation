"""Initialise a hoisted temp where it is first needed, not at the top.

Retail for func_02006d00 is

    str  r3, [r0]        ; *(u32 *)a0 = load(p+12)
    ldr  r0, [r2, #0x10] ; reload pointer
    ldrh r0, [r0, #0xe]
    str  r0, [r1]        ; *(u32 *)a1 = value
    bx   lr              ; return value  (same register, reused)

so the value loaded for the second store is reused by the return, but nothing is
loaded before the first store.  Hoisting every temp to the top of the body
reordered those accesses.  Emit each initialiser immediately before the
statement that first uses it (or before the final return when only the return
uses it), which keeps the access order and still lets later uses reuse it.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# render a setv node
old_render = """            if st[0] == 'sset':
                out.append(f'{pad}sl{st[1]} = {e(st[2])};')
                continue"""
new_render = """            if st[0] == 'sset':
                out.append(f'{pad}sl{st[1]} = {e(st[2])};')
                continue
            if st[0] == 'setv':
                out.append(f'{pad}v{st[1]} = {e(st[2])};')
                continue"""
assert t.count(old_render) == 1
t = t.replace(old_render, new_render, 1)

# record the first top-level statement that uses each node
old_loop = """    for ti, st in enumerate(fn['stmts']):
        nodes = []
        _stmt_nodes(st, nodes)
        for n in nodes:
            flat.append(n)
            tops.setdefault(repr(n), set()).add(ti)"""
new_loop = """    first_ti = {}
    for ti, st in enumerate(fn['stmts']):
        nodes = []
        _stmt_nodes(st, nodes)
        for n in nodes:
            flat.append(n)
            tops.setdefault(repr(n), set()).add(ti)
            first_ti.setdefault(repr(n), ti)"""
assert t.count(old_loop) == 1
t = t.replace(old_loop, new_loop, 1)

# insert setv statements instead of hoisting initialisers into the prologue
old_keep = """        for i, expr in enumerate(inner_exprs):
            if i in used:
                hoist_names.append(f'v{i}')
                hoist_types[f'v{i}'] = _temp_type(order[i], addr_nodes)
                hoist_assigns.append(f'    v{i} = {e(expr)};')"""
new_keep = """        by_pos = {}
        for i, expr in enumerate(inner_exprs):
            if i in used:
                hoist_names.append(f'v{i}')
                hoist_types[f'v{i}'] = _temp_type(order[i], addr_nodes)
                pos = first_ti.get(repr(order[i]), len(fn['stmts']))
                by_pos.setdefault(pos, []).append(('setv', i, expr))
        if by_pos:
            rebuilt = []
            for ti, st in enumerate(fn['stmts']):
                rebuilt.extend(by_pos.get(ti, []))
                rebuilt.append(st)
            rebuilt.extend(by_pos.get(len(fn['stmts']), []))
            fn = dict(fn)
            fn['stmts'] = rebuilt"""
assert t.count(old_keep) == 1
t = t.replace(old_keep, new_keep, 1)

old_ins = """    if decls:
        body[0:0] = decls + hoist_assigns"""
new_ins = """    if decls:
        body[0:0] = decls"""
assert t.count(old_ins) == 1
t = t.replace(old_ins, new_ins, 1)

P.write_text(t, encoding="utf-8")
print("temps initialised at first use")
