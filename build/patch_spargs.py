"""Model stack-passed arguments and frame addresses.

After `sub sp, sp, #0x44`, `[sp, #0x74]` is not a local: it is the 5th argument
on the caller's stack.  Track the allocation depth so those loads become
`a4`, `a5`, ... (the emitter already widens its signature for those nodes), and
so `add rX, sp, #off` with off inside the frame becomes `&sl_off`.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

# 1. track the frame depth instead of dropping it
old = """            elif rd == 'r13':
                pass"""
new = """            elif rd == 'r13':
                # remember how far sp was moved so [sp, #n] can be classified
                depth = regs.get('r13', ('unk',))
                step = _imm(src2) if src2.startswith('#') else None
                if step is not None and depth[0] == 'const':
                    regs['r13'] = ('const', depth[1] + (step if mn == 'sub' else -step))
                elif step is not None and depth[0] == 'unk':
                    regs['r13'] = ('const', step if mn == 'sub' else -step)
                else:
                    regs['r13'] = ('unk',)"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

# also initialise the depth register
old = """    regs0.update({f'r{n}': ('unk',) for n in range(4, 13)})"""
new = """    regs0.update({f'r{n}': ('unk',) for n in range(4, 13)})
    regs0['r13'] = ('const', 0)"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

# 2. classify sp-relative loads
old = """            if rn == 'r13':
                if mn == 'str':
                    val = regs.get(rt, ('unk',))
                    stack[off] = val
                    stmts.append(('sset', off, val))
                    continue
                if off in stack and width == 'w' and not signed:
                    regs[rt] = ('slot', off)
                    continue
                raise Bail('load from sp')"""
new = """            if rn == 'r13':
                depth = regs.get('r13', ('unk',))
                base = depth[1] if depth[0] == 'const' else None
                inside = base is not None and off < base
                if mn == 'str':
                    if not inside:
                        raise Bail('store above frame')
                    val = regs.get(rt, ('unk',))
                    stack[off] = val
                    stmts.append(('sset', off, val))
                    continue
                if base is not None and not inside:
                    # caller-passed argument: 4 + word index past the frame
                    rel = off - base
                    if width == 'w' and not signed and rel % 4 == 0:
                        regs[rt] = ('a', 4 + rel // 4)
                        continue
                    raise Bail('load from sp')
                if off in stack and width == 'w' and not signed:
                    regs[rt] = ('slot', off)
                    continue
                raise Bail('load from sp')"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

# 3. add rX, sp, #off  ->  address of a frame slot
old = """            elif rd == 'r13':"""
new = """            elif rd != 'r13' and rn == 'r13' and mn == 'add' and src2.startswith('#'):
                depth = regs.get('r13', ('unk',))
                base = depth[1] if depth[0] == 'const' else None
                if base is not None and _imm(src2) < base:
                    regs[rd] = ('slotaddr', _imm(src2))
                else:
                    regs[rd] = ('arith', '+', left, right)
            elif rd == 'r13':"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

# 4. render the slot address and declare the slot
old = """    if t == 'slot':
        return f'sl{x[1]}'"""
new = """    if t == 'slot':
        return f'sl{x[1]}'
    if t == 'slotaddr':
        return f'(u8 *)&sl{x[1]}'"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

old = """    slots = sorted({st[1] for st in _walk_stmts(fn['stmts']) if st[0] == 'sset'})"""
new = """    _all = []
    for _st in _walk_stmts(fn['stmts']):
        _stmt_nodes(_st, _all)
    slots = sorted({st[1] for st in _walk_stmts(fn['stmts']) if st[0] == 'sset'}
                   | {n[1] for n in _all if n[0] == 'slotaddr'})"""
assert t.count(old) == 1
t = t.replace(old, new, 1)

P.write_text(t, encoding="utf-8")
print("stack arguments and frame addresses modelled")
