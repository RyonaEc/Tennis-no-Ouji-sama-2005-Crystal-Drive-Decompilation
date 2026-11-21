"""Straight-line MMIO synthesizer (capstone register ids: r0=66.., sp=12, lr=10, pc=11).

Emits a candidate C body for functions that are a linear sequence of
absolute-address (hardware register) reads/writes, then optionally tests each
candidate through t10.try_unit.

usage: python build/synth.py [mod] [maxsize] [--test]
"""
import importlib.util
import pathlib
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
from capstone.arm import ARM_OP_REG, ARM_OP_IMM, ARM_OP_MEM


def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m


A = load('build/auto.py', 'auto')
T = load('build/t10.py', 't10')
MD = Cs(CS_ARCH_ARM, CS_MODE_ARM)
MD.detail = True

R0, SP, LR, PC = 66, 12, 10, 11
ARGS = ['a0', 'a1', 'a2', 'a3']
SEED = {R0: ('a', 0), R0 + 1: ('a', 1), R0 + 2: ('a', 2), R0 + 3: ('a', 3)}
BRANCHES = ('b', 'beq', 'bne', 'bge', 'blt', 'bgt', 'ble', 'bcs', 'bcc',
            'bhi', 'bls', 'bmi', 'bpl', 'bvs', 'bvc', 'bal')
HW_LO, HW_HI = 0x04000000, 0x06000000


class Unsupported(Exception):
    pass


def addr_expr(a):
    return '0x%08x' % (a & 0xffffffff)


def emit(name, addr, size):
    regs = dict(SEED)
    stores, used = [], set()
    for i in A.disasm(addr, size):
        mn, ops = i.mnemonic, i.operands
        bad = Unsupported('%s %s' % (mn, i.op_str))
        if mn == 'bx':
            if len(ops) == 1 and ops[0].reg == LR:
                continue
            raise bad
        if mn in ('push', 'pop', 'stmdb', 'ldm', 'ldmia', 'stmia'):
            continue
        if mn in ('sub', 'add') and len(ops) == 3 and ops[0].reg == SP and ops[1].reg == SP:
            continue
        if mn in BRANCHES or mn in ('cmp', 'cmn', 'tst', 'teq', 'bl', 'mla', 'mul'):
            raise bad
        if not ops:
            raise bad
        if mn in ('mov', 'mvn'):
            d, s = ops[0], ops[1]
            if d.type != ARM_OP_REG:
                raise bad
            if s.type == ARM_OP_IMM:
                regs[d.reg] = ('c', (s.imm if mn == 'mov' else ~s.imm) & 0xffffffff)
            elif s.type == ARM_OP_REG and s.reg in regs:
                v = regs[s.reg]
                if mn == 'mvn':
                    if v[0] != 'c':
                        raise bad
                    v = ('c', (~v[1]) & 0xffffffff)
                regs[d.reg] = v
            else:
                raise bad
        elif mn in ('add', 'sub', 'and', 'orr', 'bic', 'eor', 'rsb', 'lsl', 'lsr', 'asr'):
            if len(ops) != 3 or ops[0].type != ARM_OP_REG or ops[1].type != ARM_OP_REG:
                raise bad
            sv = regs.get(ops[1].reg)
            tv = regs.get(ops[2].reg) if ops[2].type == ARM_OP_REG else (
                ('c', ops[2].imm) if ops[2].type == ARM_OP_IMM else None)
            if sv is None or tv is None or sv[0] != 'c' or tv[0] != 'c':
                raise bad
            a, b = sv[1], tv[1]
            v = {'add': a + b, 'sub': a - b, 'rsb': b - a, 'and': a & b,
                 'orr': a | b, 'eor': a ^ b, 'bic': a & ~b,
                 'lsl': a << (b & 31), 'lsr': a >> (b & 31), 'asr': a >> (b & 31)}[mn]
            regs[ops[0].reg] = ('c', v & 0xffffffff)
        elif mn in ('ldr', 'ldrh', 'ldrb'):
            d = ops[0]
            if d.type != ARM_OP_REG or ops[1].type != ARM_OP_MEM:
                raise bad
            mem = ops[1].mem
            if mem.index:
                raise bad
            if mem.base == PC:
                regs[d.reg] = ('c', A._lit(i))
                continue
            base = regs.get(mem.base)
            if base is None or base[0] != 'c':
                raise bad
            a = (base[1] + mem.disp) & 0xffffffff
            width = {'ldr': 32, 'ldrh': 16, 'ldrb': 8}[mn]
            if HW_LO <= a < HW_HI:
                regs[d.reg] = ('m', '*(volatile u%d *)%s' % (width, addr_expr(a)))
            else:
                regs[d.reg] = ('c', a)
        elif mn in ('str', 'strh', 'strb'):
            if len(ops) != 2 or ops[0].type != ARM_OP_REG or ops[1].type != ARM_OP_MEM:
                raise bad
            mem = ops[1].mem
            if mem.index:
                raise bad
            base = regs.get(mem.base)
            if base is None or base[0] != 'c':
                raise bad
            a = (base[1] + mem.disp) & 0xffffffff
            if not (HW_LO <= a < HW_HI):
                raise bad
            sv = regs.get(ops[0].reg)
            if sv is None:
                raise bad
            if sv[0] == 'c':
                vx = '0x%08x' % sv[1]
            elif sv[0] == 'a':
                vx = ARGS[sv[1]]
                used.add(sv[1])
            else:
                vx = sv[1]
            stores.append(({'str': 32, 'strh': 16, 'strb': 8}[mn], a, vx))
        else:
            raise bad

    if not stores or not any(HW_LO <= a < HW_HI for _, a, _ in stores):
        return None
    if used:
        head = 'void %s(%s)' % (name, ', '.join('u32 ' + ARGS[n] for n in sorted(used)))
    else:
        head = 'void %s(void)' % name
    lines = [head, '{']
    for width, a, vx in stores:
        lines.append('    *(volatile u%d *)%s = %s;' % (width, addr_expr(a), vx))
    lines.append('}')
    return '\n'.join(lines) + '\n'


def main(argv):
    mod = argv[0] if argv else 'arm9'
    maxsize = int(argv[1], 0) if len(argv) > 1 else 0x80
    do_test = '--test' in argv
    A.set_module(mod)
    done = {q.stem for q in pathlib.Path('src/%s' % mod).glob('*.c')}
    cands = []
    for addr, (name, size) in sorted(A.FUNCS.items()):
        if name in done or size > maxsize:
            continue
        try:
            src = emit(name, addr, size)
        except Unsupported:
            src = None
        if src:
            cands.append((name, addr, size, src))
    print('%s: %d straight-line candidates (<=0x%x)' % (mod, len(cands), maxsize))
    hit = 0
    for name, addr, size, src in cands:
        if not do_test:
            print('--- %s (0x%x)\n%s' % (name, size, src))
            continue
        ok = T.try_unit(name, addr, size, src, mod=mod)
        print('   -> %s: %s' % (name, 'KEPT' if ok else 'miss'), flush=True)
        hit += 1 if ok else 0
    if do_test:
        print('%s: kept %d of %d' % (mod, hit, len(cands)))


if __name__ == '__main__':
    main(sys.argv[1:])
