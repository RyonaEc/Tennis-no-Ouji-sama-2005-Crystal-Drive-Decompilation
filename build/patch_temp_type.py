"""Give a hoisted temp the type of its expression, not always u32.

`u32 v0; v0 = *(u16*)p; if (v0 < a0)` compares unsigned, so mwcc emits `lo`
where retail emits `lt` — a u16 load promotes to signed int in C, but a u32
temp does not.  Type each temp from its expression, keeping u32 when the value
is used as an address (a narrow temp would truncate a pointer).
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# helper
anchor = "def _has_load(x):"
helper = """def _temp_type(expr, addr_nodes):
    if repr(expr) in addr_nodes:
        return 'u32'
    if expr[0] == 'load':
        return TY[(expr[3], expr[4])]
    return 'u32'


"""
assert t.count(anchor) == 1
t = t.replace(anchor, helper + anchor, 1)

# collect the expressions used as addresses
old_ret_nodes = """    ret_nodes = []"""
new_ret_nodes = """    addr_nodes = set()
    for n in flat:
        if n[0] == 'load':
            addr_nodes.add(repr(n[1]))
    for st in fn['stmts']:
        if st[0] == 'store':
            addr_nodes.add(repr(st[1]))
    ret_nodes = []"""
assert t.count(old_ret_nodes) == 1
t = t.replace(old_ret_nodes, new_ret_nodes, 1)

# typed declaration instead of a single u32 line
old_decl = """    if hoist_names:
        decls.append('    u32 ' + ', '.join(hoist_names) + ';')"""
new_decl = """    if hoist_names:
        by_type = {}
        for nm2 in hoist_names:
            by_type.setdefault(hoist_types.get(nm2, 'u32'), []).append(nm2)
        for ty2, names2 in by_type.items():
            decls.append(f'    {ty2} ' + ', '.join(names2) + ';')"""
assert t.count(old_decl) == 1
t = t.replace(old_decl, new_decl, 1)

old_names = """    hoist_names, hoist_assigns = [], []"""
new_names = """    hoist_names, hoist_assigns, hoist_types = [], [], {}"""
assert t.count(old_names) == 1
t = t.replace(old_names, new_names, 1)

old_emit = """                hoist_names.append(f'v{i}')
                hoist_assigns.append(f'    v{i} = {e(expr)};')"""
new_emit = """                hoist_names.append(f'v{i}')
                hoist_types[f'v{i}'] = _temp_type(order[i], addr_nodes)
                hoist_assigns.append(f'    v{i} = {e(expr)};')"""
assert t.count(old_emit) == 1
t = t.replace(old_emit, new_emit, 1)

P.write_text(t, encoding="utf-8")
print("typed hoisted temps")
