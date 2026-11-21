"""Two fixes around conditional runs.

1. `stack[0]` crashed with KeyError when a call takes an argument below the
   saved registers; treat a missing slot as unknown so the unit bails cleanly.
2. When the compare that governs a conditional run is no longer on the stack
   (something consumed it), rescan backwards for the last flag-setting
   instruction, the same way branch sites do.
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

old_stack = """                if stack:
                    a = a + [stack[0]]"""
new_stack = """                if stack:
                    a = a + [stack.get(0, ('unk',))]"""
assert t.count(old_stack) == 1, t.count(old_stack)
t = t.replace(old_stack, new_stack, 1)

old_cmp = """            if not cmps:
                raise Bail('conditional without compare')"""
new_cmp = """            if not cmps:
                k = i - 1
                while k >= 0 and not _sets_flags(body[k][2]):
                    k -= 1
                if k < 0:
                    raise Bail('conditional without compare')
                cmn_mn, _cmn_c, cmn_ins = body[k]
                cmn_toks = [x.strip() for x in cmn_ins.op_str.split(',')] if cmn_ins.op_str else []
                if cmn_mn not in ('cmp', 'cmn') or len(cmn_toks) < 2:
                    raise Bail('conditional without compare')
                c_left = regs.get(R(cmn_toks[0]), ('unk',))
                c_right = (('const', _imm(cmn_toks[1])) if cmn_toks[1].startswith('#')
                           else regs.get(R(cmn_toks[1]), ('unk',)))
                cmps.append((c_left, '==' if cmn_mn == 'cmp' else '!=', c_right))"""
assert t.count(old_cmp) == 1, t.count(old_cmp)
t = t.replace(old_cmp, new_cmp, 1)

P.write_text(t, encoding="utf-8")
print("compare fallback + stack arg fix")
