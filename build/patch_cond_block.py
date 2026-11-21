"""Conditional instruction runs are not always conditional epilogues.

`movne r3,#3 / strne r3,[r1]` is an `if (cond) { ... }` block, but the generator
only understood the early-return form and bailed as `cond mov`.  Handle the block
form when it is safe: the run must not branch, must not be a `pc` jump table, and
any register it writes must be dead after the run (otherwise the value would be
conditionally defined and a single expression cannot represent it).
"""
import pathlib

P = pathlib.Path(r"M:/Tennis no Ouji-sama 2005 - Crystal Drive Decomp/build/auto.py")
t = P.read_text(encoding="utf-8")

start_marker = "        if cond != 'al':\n"
end_marker = "            stmts.append(('guard', cmps.pop(), retval))\n            i = j\n            continue\n"
start = t.index(start_marker)
end = t.index(end_marker) + len(end_marker)

new = '''        if cond != 'al':
            run = [(mn, ins)]
            j = i + 1
            while j < end and body[j][1] == cond:
                run.append((body[j][0], body[j][2]))
                j += 1
            if not cmps:
                raise Bail('conditional without compare')

            # A run of conditional instructions is either a conditional epilogue
            # (early return) or a plain `if (cond) { ... }` block.
            retval = None
            ended = False
            epilogue = True
            for rmn, ri in run:
                rtoks = [t.strip() for t in ri.op_str.split(',')] if ri.op_str else []
                if rmn == 'mov' and R(rtoks[0]) == 'r0':
                    retval = ('const', _imm(rtoks[1])) if rtoks[1].startswith('#') else regs.get(R(rtoks[1]), ('unk',))
                elif rmn == 'bx':
                    if R(rtoks[0]) != 'r14':
                        raise Bail('cond bx reg')
                    ended = True
                elif rmn in ('add', 'sub'):
                    if R(rtoks[0]) != 'r13':
                        epilogue = False
                elif rmn in ('ldm', 'pop', 'ldmia'):
                    pass
                else:
                    epilogue = False
            if epilogue and ended:
                stmts.append(('guard', cmps.pop(), retval))
                i = j
                continue
            if any(rmn == 'b' for rmn, _ri in run):
                raise Bail('cond block with branch')
            if any(rmn in ('add', 'sub') and R(ri.op_str.split(',')[0]) == 'r15'
                   for rmn, ri in run):
                raise Bail('switch table')

            def _access(ri):
                try:
                    rd, wr = ri.regs_access()
                except Exception:
                    return set(), set()
                return ({ri.reg_name(x) for x in rd}, {ri.reg_name(x) for x in wr})

            defs = set()
            for _rmn, ri in run:
                _reads, writes = _access(ri)
                defs |= writes
            defs -= {'sp', 'pc'}
            live_out = False
            if defs:
                for k2 in range(j, end):
                    _m2, _c2, i2 = body[k2]
                    r2, w2 = _access(i2)
                    if defs & r2:
                        live_out = True
                        break
                    if defs & w2:
                        break
            if live_out:
                raise Bail('cond run has live result')

            cond_expr = cmps.pop()
            sub = [(m2, 'al', i2) for m2, i2 in run]
            block_stmts, term, _n, _r = exec_range(sub, 0, len(sub), dict(regs), calls, [])
            if term is not None:
                raise Bail('cond block terminator')
            stmts.append(('if', cond_expr, block_stmts, [], None, None))
            i = j
            continue
'''

P.write_text(t[:start] + new + t[end:], encoding="utf-8")
print("conditional block support added")
