"""Reconstruct C for ARM9 functions from the retail disassembly.

Handles prologue -> straight-line code (calls, tail calls through ip,
loads/stores) plus the control flow CodeWarrior emits for:

    if (c) { A }            cmp / b<inv> Lend / A / Lend:
    if (c) { A } else { B } cmp / b<inv> Lelse / A / b Lend / Lelse: B / Lend:
    if (c) return x;        cmp / <epilogue under cc>

Loops, switch tables and nested conditionals raise Bail and are left to hand
decompilation.  Every candidate is validated by rebuilding the ROM and
comparing SHA-1, so a wrong reconstruction costs only a test cycle.
"""
import pathlib, re, struct
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
IDENT_RE = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')

BASE = 0x02004000
ROM = pathlib.Path('extract/ATOJ/arm9/arm9.bin').read_bytes()
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
md.detail = True

# Every loaded module's bytes, keyed by name -> (base address, bytes).  A
# function in an overlay can reference ARM9 data and functions, so both ends
# have to be readable.
POOL = {'arm9': (0x02004000, ROM)}

def _pool(addr):
    for base, rom in POOL.values():
        if base <= addr < base + len(rom):
            return base, rom
    raise Bail(f'address {addr:#x} is outside the loaded modules')

def load_symbols(path=None):
    sym, funcs = {}, {}
    path = pathlib.Path(path or 'config/ATOJ/arm9/symbols.txt')
    for l in path.read_text().splitlines():
        m = re.match(r'(\S+) kind:function\(\w+,size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)', l)
        if m:
            funcs[int(m.group(3), 16)] = (m.group(1), int(m.group(2), 16))
            sym[int(m.group(3), 16)] = m.group(1)
            continue
        m = re.match(r'(\S+) kind:\S+ addr:(0x[0-9a-f]+)', l)
        if m:
            sym[int(m.group(2), 16)] = m.group(1)
    return sym, funcs

SYM, FUNCS = load_symbols()
NAME2ADDR = {v[0]: k for k, v in FUNCS.items()}
ALIAS = {'ip': 'r12', 'sp': 'r13', 'lr': 'r14', 'fp': 'r11', 'sl': 'r10', 'sb': 'r9'}
MODULES = ['arm9', 'itcm', 'dtcm'] + [f'ov{n:03d}' for n in range(11)]

def set_module(mod):
    """Point the analysis at one module: arm9, itcm, dtcm or ovNNN."""
    global BASE, ROM, SYM, FUNCS, NAME2ADDR
    if mod == 'arm9':
        binary = pathlib.Path('extract/ATOJ/arm9/arm9.bin')
        symp = pathlib.Path('config/ATOJ/arm9/symbols.txt')
        base = 0x02004000
    elif mod in ('itcm', 'dtcm'):
        binary = pathlib.Path(f'extract/ATOJ/arm9/arm9.bin')
        symp = pathlib.Path(f'config/ATOJ/arm9/{mod}/symbols.txt')
        base = 0x02004000
    else:
        binary = pathlib.Path(f'extract/ATOJ/arm9_overlays/{mod}.bin')
        symp = pathlib.Path(f'config/ATOJ/arm9/overlays/{mod}/symbols.txt')
        m = re.search(r'\.text\s+start:(0x[0-9a-f]+)',
                      pathlib.Path(f'config/ATOJ/arm9/overlays/{mod}/delinks.txt').read_text())
        if not m:
            raise SystemExit(f'{mod} has no .text section (empty overlay)')
        base = int(m.group(1), 16)
    rom = binary.read_bytes()
    POOL[mod] = (base, rom)
    sym, funcs = load_symbols(symp)
    if mod not in ('arm9', 'itcm', 'dtcm'):
        asy, _ = load_symbols(pathlib.Path('config/ATOJ/arm9/symbols.txt'))
        for k, v in asy.items():
            sym.setdefault(k, v)
    BASE, ROM = base, rom
    SYM, FUNCS = sym, funcs
    NAME2ADDR = {v[0]: k for k, v in funcs.items()}
    return mod

def R(tok):
    t = tok.strip()
    return ALIAS.get(t, t)

def _cond_of(ins):
    base, cond = _norm(ins.mnemonic)
    return cond


def disasm(addr, size=None):
    """Decode every instruction reachable from `addr` within the unit range."""
    if size is None:
        size = FUNCS[addr][1]
    base, rom = _pool(addr)
    end = addr + size
    seen = {}
    todo = [addr]
    while todo:
        pc = todo.pop()
        if pc in seen or not (addr <= pc < end):
            continue
        chunk = rom[pc - base: pc - base + 4]
        if len(chunk) < 4:
            continue
        decoded = list(md.disasm(chunk, pc))
        if not decoded:
            continue
        ins = decoded[0]
        seen[pc] = ins
        base_mn, cond = _norm(ins.mnemonic)
        nxt = pc + 4
        if base_mn in ('bx', 'blx') or (base_mn in ('pop', 'ldm', 'ldmia') and 'pc' in ins.op_str):
            # an unconditional return ends the path; a conditional one falls through
            if cond != 'al':
                todo.append(nxt)
            continue
        if base_mn == 'b':
            try:
                tgt = _target(ins)
            except ValueError:
                todo.append(nxt)
                continue
            if addr <= tgt < end:
                todo.append(tgt)
            if cond != 'al':
                todo.append(nxt)
            continue
        todo.append(nxt)
    return [seen[a] for a in sorted(seen)]


def _regidx(i, rid):
    n = md.reg_name(rid)
    if n and n.startswith('r') and n[1:].isdigit():
        return int(n[1:])
    return None

ARITY_CACHE = {}
# arity() reads a callee's arity to see which arguments a call passes, so a
# cycle of calls would recurse forever without this.
_ARITY_BUSY = set()

def arity(addr):
    # Keyed by module base as well as address: overlays load at ranges that
    # overlap arm9's and each other's, so a callee at the same address in
    # another module is a different function with a different arity. Keying by
    # address alone made the result depend on which module ran first, which is
    # why a lane-order change altered which units had candidates at all.
    key = (BASE, addr)
    if key in ARITY_CACHE:
        return ARITY_CACHE[key]
    if addr not in SYM:
        ARITY_CACHE[key] = 0
        return 0
    try:
        ins = disasm(addr)
    except Exception:
        ARITY_CACHE[key] = 0
        return 0
    written, used = set(), 0
    for i in ins:
        if i.mnemonic in ('bl', 'blx'):
            # A call reads the argument registers it takes before clobbering
            # them, so `bl g(a0)` with a0 never written in this function is a
            # use of a0.  Skipping that read made a pass-through wrapper look
            # like it took no arguments; its callers then emitted fewer
            # arguments than it takes, and the walker reported the missing ones
            # as unknown values.
            if i.mnemonic == 'bl':
                tgt = _target(i)
                if tgt in SYM and tgt != addr and tgt not in _ARITY_BUSY:
                    _ARITY_BUSY.add(addr)
                    try:
                        ncallee = arity(tgt)
                    finally:
                        _ARITY_BUSY.discard(addr)
                    for k in range(min(ncallee, 4)):
                        if k not in written:
                            used = max(used, k + 1)
            written.update({0, 1, 2, 3})
            continue
        r, w = i.regs_access()
        for rid in r:
            n = _regidx(i, rid)
            if n is not None and n < 4 and n not in written:
                used = max(used, n + 1)
        for rid in w:
            n = _regidx(i, rid)
            if n is not None:
                written.add(n)
    ARITY_CACHE[key] = used
    return used

TY = {('w', False): 'u32', ('w', True): 's32', ('h', False): 'u16', ('h', True): 's16',
      ('b', False): 'u8', ('b', True): 's8'}

def call_n(target, regs):
    n = arity(target)
    for k in range(4):
        v = regs.get(f'r{k}')
        if v is not None and v[0] != 'unk' and v != ('a', k):
            n = max(n, k + 1)
    return min(n, 4)

class Bail(Exception):
    pass

CONDS = ('eq', 'ne', 'cs', 'cc', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le', 'hs', 'lo')
BASES = ('mov', 'add', 'sub', 'rsb', 'ldr', 'str', 'ldrh', 'strh', 'ldrb', 'strb', 'ldrsb',
         'ldrsh', 'cmp', 'cmn', 'and', 'orr', 'eor', 'bic', 'lsl', 'lsr', 'asr', 'b', 'bl',
         'mul', 'mla', 'mvn', 'bx', 'blx', 'ldm', 'stm', 'push', 'pop',
         'smull', 'umull', 'smulbb', 'smulbt', 'smultb', 'smultt')
# the halfword multiplies set no flags, so they must never be split as SETS forms
MN_SUFFIXED = ('smulbb', 'smulbt', 'smultb', 'smultt')
# mnemonics that take an S suffix (flag-setting form) without changing semantics
SETS = ('mov', 'add', 'sub', 'rsb', 'and', 'orr', 'eor', 'bic', 'lsl', 'lsr', 'asr', 'mul', 'mvn')

def _norm(mn):
    if mn in MN_SUFFIXED:
        return mn, 'al'
    for c in CONDS:
        if mn.endswith(c) and mn[:-len(c)] in BASES:
            return mn[:-len(c)], c
    if mn.endswith('s') and mn[:-1] in SETS:
        return mn[:-1], 'al'
    for c in CONDS:
        if mn.endswith(c + 's') and mn[:-len(c) - 1] in SETS:
            return mn[:-len(c) - 1], c
    return mn, 'al'

INV = {'eq': 'ne', 'ne': 'eq', 'lt': 'ge', 'ge': 'lt', 'gt': 'le', 'le': 'gt',
       'hi': 'ls', 'ls': 'hi', 'cs': 'cc', 'cc': 'cs', 'hs': 'lo', 'lo': 'hs'}
CMPOP = {'hs': '>=', 'lo': '<', 'eq': '==', 'ne': '!=', 'lt': '<', 'le': '<=', 'gt': '>', 'ge': '>=',
         'hi': '>', 'ls': '<=', 'cs': '>=', 'cc': '<'}
# First cell id used for the results of a conditional run that outlive it.  It
# sits above any real frame offset, so a phi cell and a stack slot never collide.
_PHI_CELL = 0x10000

def _lit(ins):
    m = re.search(r'#(-?0x[0-9a-f]+|-?\d+)', ins.op_str.split('[pc')[1])
    if not m:
        if ins.op_str.strip().endswith('[pc]'):
            la = (ins.address + 8) & 0xffffffff
            base, rom = _pool(la)
            return struct.unpack_from('<I', rom, la - base)[0]
        raise Bail('pc-relative without literal offset')
    la = (ins.address + 8 + int(m.group(1), 0)) & 0xffffffff
    base, rom = _pool(la)
    return struct.unpack_from('<I', rom, la - base)[0]

def _imm(tok):
    return int(tok.strip().lstrip('#'), 0)

def _reglist(ops):
    """Register names of a `{r0, r1, ...}` operand, spaces removed."""
    return [r for r in ops[ops.index('{') + 1:ops.rindex('}')].split(',') if r]


def _frame_push(item):
    """Register names if this instruction pushes onto the stack, else None."""
    mn, _cond, ins = item
    ops = ins.op_str.replace(' ', '')
    if (mn == 'push' and ops.startswith('{')) or (mn == 'stmdb' and ops.startswith('sp!')):
        return _reglist(ops)
    return None


def _homed_frame(body):
    """sp offset of the first homed argument, or None if arguments aren't homed.

    CodeWarrior homes r0-r3 with a leading `push {r0-r3}` when a function needs
    its arguments addressable, which happens with 5+ parameters.  The home slots
    hold the argument values themselves, so a load from one of them is a read of
    that argument however the compiler meant the push.  Later prologue pushes and
    `sub sp, #n` move the home area down, hence the offset is everything pushed
    minus the 16 bytes of the four argument words.

    Only a prologue that starts by pushing r0 is homed; every other prologue
    returns None and keeps the plain stack-slot handling.
    """
    names = _frame_push(body[0])
    if not names or names[0] != 'r0':
        return None
    pushed = 4 * len(names)
    for item in body[1:]:
        names = _frame_push(item)
        ops = item[2].op_str.replace(' ', '')
        if names:
            pushed += 4 * len(names)
        elif item[0] == 'sub' and ops.startswith('sp,sp,#'):
            pushed += _imm(ops.split('#')[1])
        else:
            break
    return pushed - 16


def _sp_step(regs, delta):
    """Move the modelled sp by `delta` bytes below its entry value.

    sp carries its own kind, not a plain constant: `add r2, sp, #4` is an
    address and must never be folded with an integer.
    """
    depth = regs.get('r13', ('unk',))
    regs['r13'] = (('sp', depth[1] + delta) if depth[0] == 'sp'
                   else ('unk',))


def _frame_cell(regs, off):
    """Cell id for [sp, #off]: how many bytes below the entry sp it sits, so a
    cell keeps its identity while the prologue pushes and the epilogue pops."""
    depth = regs.get('r13', ('unk',))
    return depth[1] - off if depth[0] == 'sp' else off


def _mem(s):
    post = None
    if '],' in s:
        head, tail = s.split('],')
        post = _imm(tail)
        inner = head[1:]
    else:
        inner = s[s.index('[') + 1: s.index(']')]
    parts = [p.strip() for p in inner.split(',') if p.strip()]
    off, idx, shift = 0, None, 0
    if len(parts) > 1:
        if parts[1].startswith('#'):
            off = _imm(parts[1])
        else:
            idx = R(parts[1])          # register offset: [r0, r7]
    if len(parts) > 2 and parts[2]:
        shift = _imm(parts[2].split()[-1])   # [r0, r7, lsl #2]
    return R(parts[0]), off, post, idx, shift

def _sets_flags(ins):
    low = ins.mnemonic.lower()
    if low in ('cmp', 'cmn', 'tst', 'teq'):
        return True
    return low.endswith('s') and _norm(low[:-1])[0] in SETS


def _target(ins):
    return int(ins.op_str.lstrip('#'), 0)


def exec_range(body, i, end, regs, calls, cmps):
    """Execute body[i:end] symbolically.  Returns (stmts, term, next_i, regs)
    where term is None, ('ret', expr), ('jump', addr) or ('cbranch', cc, addr)."""
    stmts, stack = [], {}
    # Values a `push`/`stm` left in the frame, so a matching `pop`/`ldm` gives
    # the registers back their contents instead of leaving them unknown. Retail
    # uses the save-around-a-call idiom (`push {r0, r1, r2, ip, lr}` / `bl` /
    # `mov r3, r0` / `pop {r0, r1, r2, ip, lr}`) and without this every value
    # read after the call is unknown, which is the largest bail class.
    saved = {}
    pending_sf = None
    while i < end:
        mn, cond, ins = body[i]
        op = ins.op_str
        toks = [t.strip() for t in op.split(',')] if op else []

        if pending_sf is not None:
            # the S-suffixed op has executed by now, so this is its result value
            cmps.append((regs.get(pending_sf, ('unk',)), '!=', ('const', 0)))
            pending_sf = None

        if mn == 'b':
            if cond == 'al':
                return stmts, ('jump', _target(ins)), i + 1, regs
            return stmts, ('cbranch', cond, _target(ins)), i + 1, regs

        if cond != 'al':
            run = [(mn, ins)]
            j = i + 1
            while j < end and body[j][1] == cond:
                run.append((body[j][0], body[j][2]))
                j += 1
            if not cmps:
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
                cmps.append((c_left, '==' if cmn_mn == 'cmp' else '!=', c_right))

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
                    if _c2 != cond and _c2 != 'al':
                        continue
                    r2, w2 = _access(i2)
                    if defs & r2:
                        live_out = True
                        break
                    if defs & w2:
                        break
            left, _kind, right = cmps.pop()
            if left == ('unk',) or right == ('unk',):
                raise Bail('unknown compare operand')
            cond_expr = (left, CMPOP.get(cond, _kind), right)
            sub = [(m2, 'al', i2) for m2, i2 in run]
            block_stmts, term, _n, after = exec_range(sub, 0, len(sub), dict(regs), calls, [])
            if term is not None:
                raise Bail('cond block terminator')
            if live_out:
                # A predicated result that is read after the run was a local in
                # the original source (`v = a; if (c) v = b;`), which mwcc emits
                # as one predicated write into the register holding `v`.  Give
                # each such register a cell seeded with its incoming value, and
                # let the readers after the block use the cell.
                for reg in sorted(defs):
                    value = after.get(reg, ('unk',))
                    if value == ('unk',) or value == regs.get(reg):
                        continue
                    cell = _PHI_CELL + int(reg[1:])
                    prior = regs.get(reg, ('unk',))
                    if prior != ('unk',) and prior != ('slot', cell):
                        # seed the cell with the value this run can fall through
                        # with; a cell already holding it needs no copy
                        stmts.append(('sset', cell, prior))
                    block_stmts.append(('sset', cell, value))
                    after[reg] = ('slot', cell)
            regs.update(after)
            stmts.append(('if', cond_expr, block_stmts, [], None, None))
            i = j
            continue

        i += 1
        if mn in ('stmdb', 'stm', 'push', 'ldmia', 'ldm', 'pop'):
            # A stack form only moves the modelled sp; a transfer through any
            # other base copies words, which the C has to show.
            ops_s = op.replace(' ', '')
            names = _frame_push((mn, cond, ins))
            if names:
                # A push writes downward: after the step, sp's cell is the
                # lowest address, and the first register listed on `push` sits
                # at the highest address of the block.
                _sp_step(regs, 4 * len(names))
                depth = _frame_cell(regs, 0)
                for k, name in enumerate(names):
                    val = regs.get(name, ('unk',))
                    if val != ('unk',):
                        saved[depth - 4 * k] = val
                continue
            if mn == 'pop':
                rlist = _reglist(ops_s)
                cell0 = _frame_cell(regs, 0)
                for k, name in enumerate(rlist):
                    val = saved.get(cell0 - 4 * k)
                    if val is not None:
                        regs[name] = val
                _sp_step(regs, -4 * len(rlist))
                continue
            head, _sep, rest = ops_s.partition(',')
            if head.startswith('sp!'):
                # pop: the loads happen before sp moves back up
                rlist = _reglist(rest)
                cell0 = _frame_cell(regs, 0)
                for k, name in enumerate(rlist):
                    if name == 'pc':
                        continue
                    val = saved.get(cell0 - 4 * k)
                    if val is not None:
                        regs[name] = val
                _sp_step(regs, -4 * len(rlist))
                continue
            rb = R(head.rstrip('!'))
            if rb == 'r13':
                regs['r13'] = ('unk',)
                continue
            base = regs.get(rb, ('unk',))
            if base[0] == 'unk':
                raise Bail('ldm unknown base')
            rlist = _reglist(rest)
            if mn.startswith('ldm'):
                for k, name in enumerate(rlist):
                    if name == 'pc':
                        raise Bail('ldm pc')
                    regs[name] = ('load', base, 4 * k, 'w', False)
            else:
                for k, name in enumerate(rlist):
                    val = regs.get(name, ('unk',))
                    if val == ('unk',):
                        raise Bail('unknown value')
                    stmts.append(('store', base, 4 * k, 'w', False, val))
            if head.endswith('!'):
                regs[rb] = ('arith', '+', base, ('const', 4 * len(rlist)))
            continue
        if mn == 'bx':
            reg = R(toks[0])
            tgt = regs.get(reg)
            if reg == 'r14':
                if regs.get('r0', ('unk',))[0] != 'unk':
                    return stmts, ('ret', regs['r0']), i, regs
                return stmts, ('ret', None), i, regs
            elif tgt and tgt[0] == 'sym' and tgt[1] in NAME2ADDR:
                n = call_n(NAME2ADDR[tgt[1]], regs)
                a = [regs.get(f'r{k}', ('a', k)) for k in range(4)][:n]
                if stack:
                    extra = stack.get(_frame_cell(regs, 0))
                    if extra is not None:
                        a = a + [extra]
                calls.append((tgt[1], a, n))
                stmts.append(('call', len(calls) - 1))
                return stmts, ('ret', ('call', len(calls) - 1)), i, regs
            raise Bail('bx reg')
        if _sets_flags(ins) and mn not in ('cmp', 'cmn') and mn in SETS and toks:
            pending_sf = R(toks[0])
        if mn in ('cmp', 'cmn'):
            left = regs.get(R(toks[0]), ('unk',))
            right = ('const', _imm(toks[1])) if toks[1].startswith('#') else regs.get(R(toks[1]), ('unk',))
            cmps.append((left, '==' if mn == 'cmp' else '!=', right))
            continue
        if mn == 'blx':
            reg = R(toks[0])
            fn_expr = regs.get(reg, ('unk',))
            if fn_expr[0] == 'unk':
                raise Bail('blx unknown target')
            args = [regs.get(f'r{k}', ('a', k)) for k in range(4)]
            return stmts, ('ret', ('tailcall', fn_expr, args)), i, regs

        if mn == 'bl':
            target = _target(ins)
            name = SYM.get(target)
            if not name:
                raise Bail('bl target')
            n = call_n(target, regs)
            a = [regs.get(f'r{k}', ('a', k)) for k in range(4)][:n]
            if stack:
                extra = stack.get(_frame_cell(regs, 0))
                if extra is not None:
                    a = a + [extra]
            calls.append((name, a, n))
            stmts.append(('call', len(calls) - 1))
            regs['r0'] = ('call', len(calls) - 1)
            for k in (1, 2, 3, 12, 14):
                regs[f'r{k}'] = ('unk',)
            stack.clear()
            continue
        if mn == 'mul':
            rd, rn, rm = R(toks[0]), R(toks[1]), R(toks[2])
            regs[rd] = ('mul', regs.get(rn, ('unk',)), regs.get(rm, ('unk',)))
            continue
        if mn == 'mla':
            rd, rm, rs, rn = R(toks[0]), R(toks[1]), R(toks[2]), R(toks[3])
            regs[rd] = ('mla', regs.get(rm, ('unk',)), regs.get(rs, ('unk',)),
                        regs.get(rn, ('unk',)))
            continue
        if mn in ('smull', 'umull'):
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
            rd = R(toks[0])
            src = toks[1]
            regs[rd] = ('const', _imm(src)) if src.startswith('#') else regs.get(R(src), ('unk',))
            continue
        if mn == 'mvn':
            rd, src = R(toks[0]), toks[1]
            regs[rd] = ('const', ~_imm(src)) if src.startswith('#') else ('unk',)
            continue
        if mn in ('add', 'sub'):
            rd, rn, src2 = R(toks[0]), R(toks[1]), R(toks[2])
            left = regs.get(rn, ('unk',))
            right = ('const', _imm(src2)) if src2.startswith('#') else regs.get(R(src2), ('unk',))
            if rd == 'r13' and src2.startswith('#'):
                # `sub sp, #n` moves the frame n bytes further below its entry
                _sp_step(regs, _imm(src2) if mn == 'sub' else -_imm(src2))
            elif left[0] == 'const' and right[0] == 'const':
                regs[rd] = ('const', left[1] + right[1] if mn == 'add' else left[1] - right[1])
            elif rd != 'r13' and rn == 'r13' and src2.startswith('#') and left[0] == 'sp':
                # &cell, including the outgoing-argument area just above sp
                step = _imm(src2)
                cell = left[1] - step if mn == 'add' else left[1] + step
                regs[rd] = ('slotaddr', cell)
            elif rd == 'r13':
                regs['r13'] = ('unk',)
            else:
                regs[rd] = ('arith', '+' if mn == 'add' else '-', left, right)
            continue
        if mn == 'rsb':
            rd, rn, src2 = R(toks[0]), R(toks[1]), R(toks[2])
            left = regs.get(rn, ('unk',))
            right = ('const', _imm(src2)) if src2.startswith('#') else regs.get(R(src2), ('unk',))
            if left[0] == 'const' and right[0] == 'const':
                regs[rd] = ('const', right[1] - left[1])
            else:
                regs[rd] = ('arith', '-', right, left)
            continue
        if mn in ('and', 'orr', 'eor', 'bic', 'lsl', 'lsr', 'asr'):
            rd, rn, src2 = R(toks[0]), R(toks[1]), R(toks[2])
            right = ('const', _imm(src2)) if src2.startswith('#') else regs.get(R(src2), ('unk',))
            regs[rd] = ('bit', mn, regs.get(rn, ('unk',)), right)
            continue
        if mn.startswith('ldr') or mn.startswith('str'):
            rt = R(toks[0])
            memspec = op[len(toks[0]):].strip()
            if '[pc' in memspec:
                val = _lit(ins)
                name = SYM.get(val)
                regs[rt] = ('sym', name) if name else ('const', val)
                continue
            rn, off, post, idx, shift = _mem(memspec)
            width = {'ldr': 'w', 'str': 'w', 'ldrh': 'h', 'strh': 'h', 'ldrb': 'b',
                     'strb': 'b', 'ldrsb': 'b', 'ldrsh': 'h'}[mn]
            signed = mn in ('ldrsb', 'ldrsh')
            if rn == 'r13':
                depth = regs.get('r13', ('unk',))
                known = depth[0] == 'sp'
                cell = _frame_cell(regs, off)
                # Place [sp, #off] against the entry sp.  A word at or above it
                # is the caller's outgoing area, which the callee sees as its
                # 5th and later arguments; the four words just below it hold
                # r0-r3 when the prologue homes them with `push {r0-r3}`.
                rel = -cell
                argk = None
                if known and width == 'w' and not signed and rel % 4 == 0:
                    if rel >= 0:
                        if rel // 4 <= 8:
                            argk = 4 + rel // 4
                        elif mn.startswith('str'):
                            raise Bail('implausible stack argument')
                    elif _ARGHOME is not None and rel >= -16:
                        argk = (rel + 16) // 4
                if mn == 'str':
                    val = regs.get(rt, ('unk',))
                    if argk is not None:
                        stmts.append(('aset', argk, val))
                        continue
                    if known and rel >= 0:
                        raise Bail('store above frame')
                    stack[cell] = val
                    stmts.append(('sset', cell, val))
                    continue
                if argk is not None:
                    regs[rt] = ('a', argk)
                    continue
                # Any other sp-relative word is a frame cell.  A load whose
                # store this model never saw is an unknown value, not a reason
                # to lose the whole unit.
                if not known and _ARGHOME is not None:
                    home = off - _ARGHOME
                    if home in (0, 4, 8, 12):
                        regs[rt] = ('a', home // 4)
                        continue
                regs[rt] = ('slot', cell)
                continue
            base = regs.get(rn, ('unk',))
            if idx is not None:
                iv = regs.get(idx, ('unk',))
                if base[0] == 'unk' or iv[0] == 'unk':
                    raise Bail('unknown indexed base')
                if shift:
                    iv = ('bit', 'lsl', iv, ('const', shift))
                base = ('arith', '+', base, iv)
                if off:
                    base = ('arith', '+', base, ('const', off))
                    off = 0
                idx = None
            if mn.startswith('str'):
                stmts.append(('store', base, off, width, signed, regs.get(rt, ('unk',))))
            else:
                stmts.append(('setr', rt, ('load', base, off, width, signed)))
                regs[rt] = ('load', base, off, width, signed)
            if post is not None:
                regs[rn] = ('arith', '+', base, ('const', post))
            continue
        raise Bail(mn)
    return stmts, None, i, regs


def merge_regs(a, b):
    out = {}
    for k in set(a) | set(b):
        va, vb = a.get(k), b.get(k)
        out[k] = va if va == vb else ('unk',)
    return out


def _cond_at(cmps, body, upto):
    """The (left, right) operands of the compare governing a branch at index upto."""
    if cmps:
        left, op, right = cmps[-1]
        if left == ('unk',) or right == ('unk',):
            raise Bail('unknown compare operand')
        return left, right
    k = upto - 1
    while k >= 0 and not _sets_flags(body[k][2]):
        k -= 1
    if k < 0:
        raise Bail('no compare')
    return _cmp_operands(body, k)


def run_block(body, index, i, end, regs, calls, depth=0):
    """Symbolically execute body[i:end], recursing into nested control flow.

    Returns (stmts, regs); returns appear as ('return', expr) statements so they
    can sit inside any nesting level.
    """
    if depth > 40:
        raise Bail('nesting too deep')
    cmps = []
    stmts, term, i2, regs = exec_range(body, i, end, dict(regs), calls, cmps)

    if term is None:
        return stmts, regs
    if term[0] == 'ret':
        stmts.append(('return', term[1]))
        return stmts, regs
    if term[0] == 'jump':
        tgt = index.get(term[1])
        if tgt is None:
            raise Bail('branch target outside body')
        if tgt >= i2:
            rest, regs = run_block(body, index, tgt, end, regs, calls, depth + 1)
            return stmts + rest, regs
        raise Bail('backward unconditional jump')

    cc, target = term[1], term[2]
    tgt = index.get(target)
    if tgt is None:
        raise Bail('branch target outside body')
    left, right = _cond_at(cmps, body, i2)

    if tgt <= i2:
        # Backward conditional branch: the body runs up to the branch, then the
        # branch itself repeats the loop while its condition holds.
        if body[i2 - 1][0] != 'b':
            raise Bail('backward branch without terminator')
        inner, _ = run_block(body, index, tgt, i2 - 1, dict(regs), calls, depth + 1)
        stmts.append(('loop', (left, CMPOP[cc], right), inner))
        after = {k: ('unk',) for k in regs}
        rest, regs = run_block(body, index, i2, end, after, calls, depth + 1)
        return stmts + rest, regs

    prev = body[tgt - 1] if tgt - 1 >= i2 else None
    if prev and prev[0] == 'b' and prev[1] == 'al':
        merge = index.get(_target(prev[2]))
    else:
        merge = None
    if merge is not None and merge >= tgt:
        then_stmts, then_regs = run_block(body, index, i2, tgt - 1, dict(regs), calls, depth + 1)
        else_stmts, else_regs = run_block(body, index, tgt, merge, dict(regs), calls, depth + 1)
        stmts.append(('if', (left, CMPOP[INV[cc]], right), then_stmts, else_stmts, None, None))
        regs = merge_regs(then_regs, else_regs)
    else:
        merge = tgt
        then_stmts, then_regs = run_block(body, index, i2, tgt, dict(regs), calls, depth + 1)
        stmts.append(('if', (left, CMPOP[INV[cc]], right), then_stmts, [], None, None))
        regs = merge_regs(regs, then_regs)

    rest, regs = run_block(body, index, merge, end, dict(regs), calls, depth + 1)
    return stmts + rest, regs


def gen(addr, size=None):
    if size is None:
        size = FUNCS[addr][1]
    ins = disasm(addr, size)
    if not ins:
        return None
    body = []
    for i in ins:
        base, cond = _norm(i.mnemonic)
        if base not in ('stmdb', 'stm', 'ldmia', 'ldm', 'push', 'pop') + BASES:
            raise Bail(i.mnemonic)
        body.append((base, cond, i))

    global _ARGHOME
    _ARGHOME = _homed_frame(body)

    index = {i.address: n for n, (_, _, i) in enumerate(body)}
    regs0 = {f'r{n}': ('a', n) for n in range(4)}
    regs0.update({f'r{n}': ('unk',) for n in range(4, 13)})
    regs0['r13'] = ('sp', 0)

    calls = []
    stmts, _regs = run_block(body, index, 0, len(body), dict(regs0), calls)
    ret = None
    if stmts and stmts[-1][0] == 'return':
        ret = stmts[-1][1]
        stmts = stmts[:-1]
    used = _nargs(stmts, calls, ret)
    return dict(addr=addr, size=size, calls=calls, stmts=stmts, ret=ret, nargs=used)


def _cmp_operands(body, k):
    """Evaluate the cmp at body[k] by running the prefix from the entry state."""
    regs = {f'r{n}': ('a', n) for n in range(4)}
    regs.update({f'r{n}': ('unk',) for n in range(4, 13)})
    regs['r13'] = ('sp', 0)
    calls, cmps = [], []
    exec_range(body, 0, k + 1, regs, calls, cmps)
    if not cmps:
        raise Bail('no compare state')
    left, op, right = cmps[-1]
    return left, right


def _walk_stmts(items):
    for st in items:
        yield st
        if st[0] == 'if':
            yield from _walk_stmts(st[2]); yield from _walk_stmts(st[3])
        elif st[0] == 'loop':
            yield from _walk_stmts(st[2])
        elif st[0] == 'setr':
            yield st


def _nargs(stmts, calls, ret):
    used = 0

    def scan(x):
        nonlocal used
        if not x:
            return
        t = x[0]
        if t == 'a':
            used = max(used, x[1] + 1)
        elif t == 'load':
            scan(x[1])
        elif t in ('arith', 'bit'):
            scan(x[2]); scan(x[3])
        elif t == 'mul':
            scan(x[1]); scan(x[2])
        elif t == 'mla':
            scan(x[1]); scan(x[2]); scan(x[3]); scan(x[4]) if len(x) > 4 else None
        elif t == 'call':
            for a in calls[x[1]][1]:
                scan(a)
        elif t == 'tailcall':
            scan(x[1])
            for a in x[2]:
                scan(a)

    def walk(items):
        nonlocal used
        for st in items:
            if st[0] == 'store':
                scan(st[1]); scan(st[5])
            elif st[0] == 'sset':
                scan(st[2])
            elif st[0] == 'setv':
                scan(st[2])
            elif st[0] == 'aset':
                scan(st[2])
                used = max(used, st[1] + 1)
            elif st[0] == 'return':
                scan(st[1])
            elif st[0] == 'call' or st[0] == 'guard':
                scan(st)
            elif st[0] == 'if':
                scan(st[1][0]); scan(st[1][2])
                walk(st[2]); walk(st[3])
                scan(st[4]); scan(st[5])
            elif st[0] == 'loop':
                scan(st[1][0]); scan(st[1][2])
                walk(st[2])
            elif st[0] == 'setr':
                scan(st[2])
    walk(stmts)
    scan(ret)
    return used


def e(x):
    if isinstance(x, tuple) and x and x[0] == 'le':
        return f'v{x[1]}'
    if x is None or x == ('unk',):
        raise Bail('unknown value')
    t = x[0]
    if t == 'a':
        return f'a{x[1]}'
    if t == 'const':
        v = x[1]
        return hex(v & 0xffffffff) if v >= 0 else f'({v})'
    if t == 'sym':
        return f'(u32){x[1]}'
    if t == 'slot':
        return f'sl{x[1]}'
    if t == 'slotaddr':
        # an address held in a u32 local: mwcc rejects assigning u8 * to u32,
        # and every consumer casts it back to a byte pointer anyway
        return f'(u32)&sl{x[1]}'
    if t == 'call':
        return f't{x[1]}'
    if t == 'tailcall':
        _, fn_expr, args = x
        sig = ', '.join('u32' for _ in args) or 'void'
        return f'((u32 (*)({sig})){e(fn_expr)})({", ".join(e(a) for a in args)})'
    if t == 'load':
        _, base, off, width, signed = x
        key = _field_key(base, off)
        if key and key[1] in _CUR_FIELDS and _CUR_FIELDS[key[1]] == (width, signed):
            return f'(({_CUR_TNAME} *)a{key[0]})->f0x{key[1]:x}'
        ty = TY[(width, signed)]
        b = f'(u8 *)({e(base)})' if base[0] != 'sym' else base[1]
        return f'*({ty} *)({b} + {off})' if off else f'*({ty} *)({b})'
    if t == 'arith':
        _, op, l, r = x
        return f'({e(l)} {op} {e(r)})'
    if t == 'bit':
        _, op, l, r = x
        if op == 'bic':
            return f'({e(l)} & ~{e(r)})'
        sym = {'and': '&', 'orr': '|', 'eor': '^', 'lsl': '<<', 'lsr': '>>', 'asr': '>>'}[op]
        return f'({e(l)} {sym} {e(r)})'
    if t == 'mul':
        _, l, r = x
        return f'({e(l)} * {e(r)})'
    if t == 'mla':
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
        return f'((s32)((s16)({left})) * (s32)((s16)({right})))'
    raise Bail(t)



def _all_nodes(x, acc):
    if not isinstance(x, tuple) or not x:
        return acc
    acc.append(x)
    t = x[0]
    if t == 'load':
        _all_nodes(x[1], acc)
    elif t in ('arith', 'bit'):
        _all_nodes(x[2], acc)
        _all_nodes(x[3], acc)
    elif t == 'mul':
        _all_nodes(x[1], acc)
        _all_nodes(x[2], acc)
    elif t == 'mla':
        for a in x[1:5]:
            _all_nodes(a, acc)
    elif t == 'mull':
        _all_nodes(x[3], acc)
        _all_nodes(x[4], acc)
    elif t == 'smul':
        _all_nodes(x[3], acc)
        _all_nodes(x[4], acc)
    elif t == 'tailcall':
        _all_nodes(x[1], acc)
        for a in x[2]:
            _all_nodes(a, acc)
    return acc


def _temp_type(expr, addr_nodes):
    if repr(expr) in addr_nodes:
        return 'u32'
    if expr[0] == 'load':
        return TY[(expr[3], expr[4])]
    return 'u32'


def _has_load(x):
    return any(n[0] == 'load' for n in _all_nodes(x, []))


def _subst(x, subs):
    if not isinstance(x, tuple) or not x:
        return x
    if repr(x) in subs:
        return ('le', subs[repr(x)])
    t = x[0]
    if t == 'load':
        return ('load',) + (_subst(x[1], subs),) + tuple(x[2:])
    if t in ('arith', 'bit'):
        return (t, x[1], _subst(x[2], subs), _subst(x[3], subs))
    if t == 'mul':
        return ('mul', _subst(x[1], subs), _subst(x[2], subs))
    if t == 'mla':
        return ('mla',) + tuple(_subst(a, subs) for a in x[1:5])
    if t == 'mull':
        return ('mull', x[1], x[2], _subst(x[3], subs), _subst(x[4], subs))
    if t == 'smul':
        return ('smul', x[1], x[2], _subst(x[3], subs), _subst(x[4], subs))
    if t == 'tailcall':
        return ('tailcall', _subst(x[1], subs), [_subst(a, subs) for a in x[2]])
    if t == 'store':
        return ('store', _subst(x[1], subs)) + tuple(x[2:5]) + (_subst(x[5], subs),)
    if t == 'sset':
        return ('sset', x[1], _subst(x[2], subs))
    if t == 'guard':
        return ('guard', x[1], _subst(x[2], subs) if x[2] is not None else None)
    if t == 'if':
        cond = (x[1][0], x[1][1], _subst(x[1][2], subs))
        cond = (_subst(cond[0], subs), cond[1], cond[2])
        return ('if', cond, [_subst(s, subs) for s in x[2]],
                [_subst(s, subs) for s in x[3]],
                _subst(x[4], subs) if x[4] is not None else None,
                _subst(x[5], subs) if x[5] is not None else None)
    if t == 'setr':
        return ('setr', x[1], _subst(x[2], subs))
    if t == 'loop':
        lcond = (_subst(x[1][0], subs), x[1][1], _subst(x[1][2], subs))
        return ('loop', lcond, [_subst(s, subs) for s in x[2]])
    return x


def arg_e(x):
    s = e(x)
    return f'(u32){s}' if x and x[0] == 'sym' else s


def _field_key(base, off):
    """Return (parameter index, byte offset) for a field access, else None."""
    if not base:
        return None
    if base[0] == 'a':
        return base[1], off
    if base[0] == 'arith' and base[1] == '+' and base[3][0] == 'const' and base[2][0] == 'a':
        return base[2][1], base[3][1] + off
    return None


_CUR_FIELDS = {}
_CUR_TNAME = None
_ARGHOME = None   # sp offset of the first homed argument word, None if args aren't homed


def _infer_struct(fn):
    """Collect field offsets touched on parameter bases."""
    fields, conflict = {}, False

    def note(base, off, width, signed):
        nonlocal conflict
        key = _field_key(base, off)
        if not key:
            return
        off = key[1]
        if off in fields and fields[off] != (width, signed):
            conflict = True
        fields[off] = (width, signed)

    def walkx(x):
        if not x:
            return
        t = x[0]
        if t == 'load':
            _, base, off, width, signed = x
            note(base, off, width, signed)
            walkx(base)
        elif t in ('arith', 'bit'):
            walkx(x[2]); walkx(x[3])
        elif t == 'mul':
            walkx(x[1]); walkx(x[2])
        elif t == 'mla':
            walkx(x[1]); walkx(x[2]); walkx(x[3])
        elif t == 'call':
            for a in fn['calls'][x[1]][1]:
                walkx(a)
        elif t == 'tailcall':
            walkx(x[1])
            for a in x[2]:
                walkx(a)

    def walk(items):
        for st in items:
            if st[0] == 'store':
                note(st[1], st[2], st[3], st[4]); walkx(st[1]); walkx(st[5])
            elif st[0] == 'sset':
                walkx(st[2])
            elif st[0] in ('call', 'guard'):
                walkx(st)
            elif st[0] == 'if':
                walkx(st[1][0]); walkx(st[1][2]); walk(st[2]); walk(st[3])
                walkx(st[4]); walkx(st[5])
            elif st[0] == 'loop':
                walkx(st[1][0]); walkx(st[1][2]); walk(st[2])

    walk(fn['stmts'])
    walkx(fn['ret'])
    if conflict or not fields:
        return None, None
    return fields, 'S_' + SYM[fn['addr']].split('_', 1)[-1]


def _struct_text(fields, tname):
    out, cur = ['typedef struct {'], 0
    for off in sorted(fields):
        width, signed = fields[off]
        if off < cur:
            return None
        if off > cur:
            out.append(f'    u8 pad_{cur}[{off - cur}];')
        ty = ('s' if signed else 'u') + {'w': '32', 'h': '16', 'b': '8'}[width]
        out.append(f'    {ty} f0x{off:x};')
        cur = off + {'w': 4, 'h': 2, 'b': 1}[width]
    out.append(f'}} {tname};')
    return '\n'.join(out)


def emit(fn):
    global _CUR_FIELDS, _CUR_TNAME
    name = SYM[fn['addr']]
    struct_fields, struct_name = _infer_struct(fn)
    struct_decl = _struct_text(struct_fields, struct_name) if struct_fields else None
    _CUR_FIELDS, _CUR_TNAME = ({}, None) if struct_decl is None else (struct_fields, struct_name)
    fields, tname = _CUR_FIELDS, _CUR_TNAME
    args = ', '.join(f'u32 a{i}' for i in range(fn['nargs'])) or 'void'
    ret_used = fn['ret'] is not None
    rtype = 'u32' if ret_used else 'void'
    protos, syms = [], set()

    def collect(x):
        if not x:
            return
        t = x[0]
        if t == 'sym':
            syms.add(x[1])
        elif t == 'load':
            collect(x[1])
        elif t in ('arith', 'bit'):
            collect(x[2]); collect(x[3])
        elif t == 'mul':
            collect(x[1]); collect(x[2])
        elif t == 'mull':
            collect(x[3]); collect(x[4])
        elif t == 'smul':
            collect(x[3]); collect(x[4])
        elif t == 'mla':
            collect(x[1]); collect(x[2]); collect(x[3]); collect(x[4]) if len(x) > 4 else None
        elif t == 'guard':
            collect(x[1][0])
            collect(x[1][2])
            if x[2] is not None:
                collect(x[2])
        elif t == 'call':
            for a in fn['calls'][x[1]][1]:
                collect(a)
        elif t == 'tailcall':
            collect(x[1])
            for a in x[2]:
                collect(a)

    def walk(items):
        for st in items:
            if st[0] == 'store':
                collect(st[1]); collect(st[5])
            elif st[0] == 'sset':
                collect(st[2])
            elif st[0] == 'return':
                collect(st[1])
            elif st[0] in ('call', 'guard'):
                collect(st)
            elif st[0] == 'if':
                collect(st[1][0]); collect(st[1][2])
                walk(st[2]); walk(st[3])
                collect(st[4]); collect(st[5])
            elif st[0] == 'loop':
                collect(st[1][0]); collect(st[1][2])
                walk(st[2])
    walk(fn['stmts'])
    collect(fn['ret'])

    for cn, _ca, _nar in fn['calls']:
        if not IDENT_RE.match(cn):
            raise Bail('non-identifier call target')
    counts = {}
    for cn, ca, _nar in fn['calls']:
        counts.setdefault(cn, set()).add(len(ca))
    for cn, ns in counts.items():
        if len(ns) == 1:
            n = ns.pop()
            protos.append(f'extern u32 {cn}({", ".join("u32" for _ in range(n)) or "void"});')
        else:
            protos.append(f'extern u32 {cn}();')
    call_names = {cn for cn, _ca, _nar in fn['calls']}
    for s in sorted(syms):
        if s in call_names:
            continue
        if not IDENT_RE.match(s):
            raise Bail('non-identifier data symbol')
        protos.append(f'extern u8 {s}[];')

    hoist_names, hoist_assigns, hoist_types = [], [], {}
    flat = []
    tops = {}

    def _stmt_nodes(st, acc):
        if st[0] == 'store':
            _all_nodes(st[1], acc)
            _all_nodes(st[5], acc)
        elif st[0] == 'sset':
            _all_nodes(st[2], acc)
        elif st[0] == 'guard':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            if st[2] is not None:
                _all_nodes(st[2], acc)
        elif st[0] == 'if':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            for s2 in list(st[2]) + list(st[3]):
                _stmt_nodes(s2, acc)
            if st[4] is not None:
                _all_nodes(st[4], acc)
            if st[5] is not None:
                _all_nodes(st[5], acc)
        elif st[0] == 'loop':
            _all_nodes(st[1][0], acc)
            _all_nodes(st[1][2], acc)
            for s2 in st[2]:
                _stmt_nodes(s2, acc)

    first_ti = {}
    for ti, st in enumerate(fn['stmts']):
        nodes = []
        _stmt_nodes(st, nodes)
        for n in nodes:
            flat.append(n)
            tops.setdefault(repr(n), set()).add(ti)
            first_ti.setdefault(repr(n), ti)
    addr_nodes = set()
    for n in flat:
        if n[0] == 'load':
            addr_nodes.add(repr(n[1]))
    for st in fn['stmts']:
        if st[0] == 'store':
            addr_nodes.add(repr(st[1]))
    ret_nodes = []
    _all_nodes(fn['ret'], ret_nodes)
    for n in ret_nodes:
        flat.append(n)
        tops.setdefault(repr(n), set()).add(len(fn['stmts']))
    arg_nodes = []
    for cn, ca, _nar in fn['calls']:
        for a in ca:
            _all_nodes(a, arg_nodes)
    all_nodes = flat + arg_nodes
    argn = max(fn['nargs'], max([n[1] for n in all_nodes if n[0] == 'a'] or [-1]) + 1)
    args = ', '.join(f'u32 a{i}' for i in range(argn)) or 'void'
    seen_count = {}
    for n in flat:
        seen_count[repr(n)] = seen_count.get(repr(n), 0) + 1
    subs, order = {}, []
    # Retail re-dereferences a pointer before every use instead of keeping it in
    # a register, so hoisting an address load is wrong.  The one value it does
    # keep is a load that is both stored and returned: `v = *p; *q = v; return v;`
    branch_free = not any(st[0] in ('if', 'loop') for st in _walk_stmts(fn['stmts']))
    if branch_free:
        store_vals = {repr(st[5]) for st in fn['stmts'] if st[0] == 'store'}
        ret_set = {repr(n) for n in ret_nodes}
        for n in flat:
            if (n[0] == 'load'
                    and seen_count[repr(n)] >= 2
                    and repr(n) not in subs
                    and repr(n) not in addr_nodes
                    and repr(n) in store_vals
                    and repr(n) in ret_set):
                subs[repr(n)] = len(order)
                order.append(n)
    if order:
        inner_exprs = [_subst(n, {k: v for k, v in subs.items() if v < i})
                       for i, n in enumerate(order)]
        fn = dict(fn)
        fn['stmts'] = [_subst(st, subs) for st in fn['stmts']]
        fn['ret'] = _subst(fn['ret'], subs)
        used = set()
        live = []
        def _live_nodes(st):
            if st[0] == 'store':
                _all_nodes(st[1], live)
                _all_nodes(st[5], live)
            elif st[0] == 'sset':
                _all_nodes(st[2], live)
            elif st[0] == 'setr':
                _all_nodes(st[2], live)
            elif st[0] == 'guard':
                _all_nodes(st[1][0], live)
                _all_nodes(st[1][2], live)
                if st[2] is not None:
                    _all_nodes(st[2], live)
            elif st[0] == 'if':
                _all_nodes(st[1][0], live)
                _all_nodes(st[1][2], live)
                for s2 in list(st[2]) + list(st[3]):
                    _live_nodes(s2)
                if st[4] is not None:
                    _all_nodes(st[4], live)
                if st[5] is not None:
                    _all_nodes(st[5], live)
            elif st[0] == 'loop':
                _all_nodes(st[1][0], live)
                _all_nodes(st[1][2], live)
                for s2 in st[2]:
                    _live_nodes(s2)
        for st in fn['stmts']:
            _live_nodes(st)
        _all_nodes(fn['ret'], live)
        for n in live:
            if n[0] == 'le':
                used.add(n[1])
        changed = True
        while changed:
            changed = False
            for i in list(used):
                for n2 in _all_nodes(inner_exprs[i], []):
                    if n2[0] == 'le' and n2[1] not in used:
                        used.add(n2[1])
                        changed = True
        by_pos = {}
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
            fn['stmts'] = rebuilt

    def render(items, depth):
        out = []
        pad = '    ' * (depth + 1)
        for st in items:
            if st[0] == 'return':
                out.append(f'{pad}return;' if (st[1] is None or rtype == 'void')
                           else f'{pad}return {e(st[1])};')
                continue
            if st[0] == 'setr':
                continue
            if st[0] == 'sset':
                out.append(f'{pad}sl{st[1]} = {e(st[2])};')
                continue
            if st[0] == 'aset':
                out.append(f'{pad}a{st[1]} = {e(st[2])};')
                continue
            if st[0] == 'setv':
                out.append(f'{pad}v{st[1]} = {e(st[2])};')
                continue
            if st[0] == 'call':
                cn, ca, _nar = fn['calls'][st[1]]
                out.append(f'{pad}t{st[1]} = {cn}({", ".join(arg_e(a) for a in ca)});')
                continue
            if st[0] == 'guard':
                (lhs, op, rhs), retval = st[1], st[2]
                cond = f'{e(lhs)} {op} {e(rhs)}'
                if rtype == 'void':
                    out.append(f'{pad}if ({cond}) return;')
                elif retval is None:
                    out.append(f'{pad}if ({cond}) return 0;')
                else:
                    out.append(f'{pad}if ({cond}) return {e(retval)};')
                continue
            if st[0] == 'loop':
                (lhs, op, rhs), inner = st[1], st[2]
                out.append(f'{pad}do {{')
                out.extend(render(inner, depth + 1))
                out.append(f'{pad}}} while ({e(lhs)} {op} {e(rhs)});')
                continue
            if st[0] == 'if':
                (lhs, op, rhs), then_s, else_s, ret_then, ret_else = st[1], st[2], st[3], st[4], st[5]
                cond = f'{e(lhs)} {op} {e(rhs)}'
                out.append(f'{pad}if ({cond}) {{')
                body_then = list(then_s)
                if ret_then is not None:
                    body_then.append(('ret', ret_then))
                out.extend(render(body_then, depth + 1))
                if else_s or ret_else is not None:
                    out.append(f'{pad}}} else {{')
                    body_else = list(else_s)
                    if ret_else is not None:
                        body_else.append(('ret', ret_else))
                    out.extend(render(body_else, depth + 1))
                out.append(f'{pad}}}')
                continue
            if st[0] == 'ret':
                out.append(f'{pad}return;' if (st[1] is None or rtype == 'void')
                           else f'{pad}return {e(st[1])};')
                continue
            _, base, off, width, signed, val = st
            ty = TY[(width, signed)]
            key = _field_key(base, off)
            if key and key[1] in fields and fields[key[1]] == (width, signed):
                dst = f'(({tname} *)a{key[0]})->f0x{key[1]:x}'
            else:
                b = f'(u8 *)({e(base)})' if base[0] != 'sym' else base[1]
                dst = f'*({ty} *)({b} + {off})' if off else f'*({ty} *)({b})'
            out.append(f'{pad}{dst} = {e(val)};')
        return out

    if (len(fn['stmts']) == 1 and fn['stmts'][0][0] == 'call'
            and fn['ret'] == ('call', fn['stmts'][0][1])):
        idx = fn['stmts'][0][1]
        cn, ca, _nar = fn['calls'][idx]
        args_s = ', '.join(arg_e(a) for a in ca)
        protos_s = '\n'.join(protos)
        # A tail call can take the address of a caller-provided frame cell
        # (`(u32)&sl12`), which no statement walked here declares. Undeclared,
        # mwcc aborts and the unit scores as "no candidate".
        decls_s = ''.join('    u32 sl%d;\n' % n for n in
                          sorted({int(_m) for _m in re.findall(r'\bsl(\d+)\b', args_s)}))
        src = (f'{rtype} {name}({args})\n{{\n{decls_s}'
               f'    return {cn}({args_s});\n}}\n')
        pre = (protos_s + '\n\n') if protos_s else ''
        return pre + src

    body = render(fn['stmts'], 0)
    decls = []
    if fn['calls']:
        decls.append('    u32 ' + ', '.join(f't{i}' for i in range(len(fn['calls']))) + ';')
    if hoist_names:
        by_type = {}
        for nm2 in hoist_names:
            by_type.setdefault(hoist_types.get(nm2, 'u32'), []).append(nm2)
        for ty2, names2 in by_type.items():
            decls.append(f'    {ty2} ' + ', '.join(names2) + ';')
    if decls:
        body[0:0] = decls
    # Declare the slots in the order the statements first use them: mwcc colours
    # by declaration order, so a list that disagrees with the statement order
    # swaps address registers (ov003 0x020b732c was two slots out for that alone).
    slots, _seen_slots = [], set()

    def _use_slots(nodes):
        for _n in nodes:
            if _n[0] in ('slotaddr', 'slot') and _n[1] not in _seen_slots:
                _seen_slots.add(_n[1])
                slots.append(_n[1])

    for _st in _walk_stmts(fn['stmts']):
        _nodes = []
        _stmt_nodes(_st, _nodes)
        _use_slots(_nodes)
        if _st[0] == 'sset' and _st[1] not in _seen_slots:
            _seen_slots.add(_st[1])
            slots.append(_st[1])
    _ret_nodes = []
    _all_nodes(fn['ret'], _ret_nodes)
    _use_slots(_ret_nodes)
    for _cn, _ca, _nar in fn['calls']:
        for _a in _ca:
            _arg_nodes = []
            _all_nodes(_a, _arg_nodes)
            _use_slots(_arg_nodes)
    # One declaration per line: `respell.py` reorders a declaration block as
    # written, and a comma list is a single line it can only reorder as a unit.
    if slots:
        body[0:0] = [f'    u32 sl{s};' for s in slots]
    if ret_used:
        body.append(f'    return {e(fn["ret"])};')
    src = f'{rtype} {name}({args})\n{{\n' + ('\n'.join(body) if body else '') + '\n}\n'
    # A slot the body addresses but never assigns (a caller-provided frame cell)
    # is missed by the collection above, which records only slots it sees
    # assigned or taken by address in the statements it walks, and `e()` can
    # inline a call whose argument mentions one. Undeclared, mwcc aborts and the
    # unit scores as "no candidate" rather than as the near miss it is, so
    # declare whatever the finished text mentions and rebuild.
    _miss = [n for n in sorted({int(_m) for _m in re.findall(r'\bsl(\d+)\b', src)})
             if n not in slots]
    if _miss:
        body[0:0] = [f'    u32 sl{n};' for n in _miss]
        src = f'{rtype} {name}({args})\n{{\n' + ('\n'.join(body) if body else '') + '\n}\n'
    head = '\n'.join(protos)
    if struct_decl:
        head = (head + '\n\n' if head else '') + struct_decl
    return head + '\n\n' + src
