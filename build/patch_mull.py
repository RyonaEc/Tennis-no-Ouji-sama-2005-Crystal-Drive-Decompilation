"""Support the 64-bit and halfword multiply instructions.

`smull rdlo, rdhi, rm, rs` writes a 64-bit product into two registers; retail
uses it for scaled fixed-point maths.  Model it as one node with a part
selector so either half can be rendered, and render as a widened C multiply.
`smulbb` and friends multiply selected halfwords.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# --- symbolic execution -------------------------------------------------
old_exec = """        if mn == 'mov':
            rd = R(toks[0])"""
new_exec = """        if mn in ('smull', 'umull'):
            rdlo, rdhi, rm, rs = (R(tk) for tk in toks[:4])
            a = regs.get(rm, ('unk',))
            b = regs.get(rs, ('unk',))
            kind = 's' if mn == 'smull' else 'u'
            regs[rdlo] = ('mull', kind, 'lo', a, b)
            regs[rdhi] = ('mull', kind, 'hi', a, b)
            continue
        if mn in ('smulbb', 'smulbt', 'smultb', 'smultt'):
            rd, rn, rm = R(toks[0]), R(toks[1]), R(toks[2])
            regs[rd] = ('smul', mn[-2], mn[-1],
                        regs.get(rn, ('unk',)), regs.get(rm, ('unk',)))
            continue
        if mn == 'mov':
            rd = R(toks[0])"""
assert t.count(old_exec) == 1
t = t.replace(old_exec, new_exec, 1)

# --- rendering ----------------------------------------------------------
old_e = """    if t == 'mla':
        _, a1, a2, a3 = x
        return f'({e(a1)} * {e(a2)} + {e(a3)})'"""
new_e = """    if t == 'mla':
        _, a1, a2, a3 = x
        return f'({e(a1)} * {e(a2)} + {e(a3)})'
    if t == 'mull':
        _, kind, part, l_op, r_op = x
        prod = f'(({kind}64)({e(l_op)}) * ({kind}64)({e(r_op)}))'
        return prod if part == 'lo' else f'((u32)({prod} >> 32))'
    if t == 'smul':
        _, hi_l, hi_r, l_op, r_op = x
        left = f'({e(l_op)})' if hi_l == 'b' else f'(({e(l_op)}) >> 16)'
        right = f'({e(r_op)})' if hi_r == 'b' else f'(({e(r_op)}) >> 16)'
        return f'((s32)((s16)({left})) * (s32)((s16)({right})))'"""
assert t.count(old_e) == 1
t = t.replace(old_e, new_e, 1)

# --- node walkers -------------------------------------------------------
old_nodes = """    elif t == 'mla':
        for a in x[1:5]:
            _all_nodes(a, acc)"""
new_nodes = """    elif t == 'mla':
        for a in x[1:5]:
            _all_nodes(a, acc)
    elif t == 'mull':
        _all_nodes(x[3], acc)
        _all_nodes(x[4], acc)
    elif t == 'smul':
        _all_nodes(x[3], acc)
        _all_nodes(x[4], acc)"""
assert t.count(old_nodes) == 1
t = t.replace(old_nodes, new_nodes, 1)

old_subst = """    if t == 'mla':
        return ('mla',) + tuple(_subst(a, subs) for a in x[1:5])"""
new_subst = """    if t == 'mla':
        return ('mla',) + tuple(_subst(a, subs) for a in x[1:5])
    if t == 'mull':
        return ('mull', x[1], x[2], _subst(x[3], subs), _subst(x[4], subs))
    if t == 'smul':
        return ('smul', x[1], x[2], _subst(x[3], subs), _subst(x[4], subs))"""
assert t.count(old_subst) == 1
t = t.replace(old_subst, new_subst, 1)

old_collect = """        elif t == 'mla':
            collect(x[1]); collect(x[2]); collect(x[3])"""
new_collect = """        elif t == 'mull':
            collect(x[3]); collect(x[4])
        elif t == 'smul':
            collect(x[3]); collect(x[4])
        elif t == 'mla':
            collect(x[1]); collect(x[2]); collect(x[3])"""
if t.count(old_collect) == 1:
    t = t.replace(old_collect, new_collect, 1)

P.write_text(t, encoding="utf-8")
print("mull/smul supported")
