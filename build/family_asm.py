"""Match intrinsic stubs (mcr/mrc/swp/mrs/msr/udf) via CodeWarrior inline asm.

These instructions have no C spelling, so the compiler-generated code around them
cannot be reproduced from expressions; an `asm { ... }` block inside a void
function emits them verbatim and lets the compiler add the `bx lr` epilogue.
"""
import importlib.util, pathlib, re, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('build/auto.py', 'auto')
t10 = load('build/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 40
max_size = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x40
mods = sys.argv[3:] or ['arm9', 'ov002']

EXOTIC = {'mcr', 'mrc', 'swp', 'swpb', 'mrs', 'msr', 'udf'}
PLAIN = {'and', 'orr', 'eor', 'mov', 'mvn', 'lsl', 'lsr', 'asr', 'add', 'sub', 'ldr',
         'str', 'ldrb', 'strb', 'ldrh', 'strh', 'bx', 'push', 'pop', 'ldm', 'stm',
         'cmp', 'bic'}


def parse(addr, size):
    ins = a.disasm(addr, size)
    if not ins or 'bx' != ins[-1].mnemonic or ins[-1].op_str.strip() != 'lr':
        return None
    body = ins[:-1]
    if not body or not any(i.mnemonic in EXOTIC for i in body):
        return None
    if any(i.mnemonic not in EXOTIC | PLAIN for i in body):
        return None
    if any(i.mnemonic in ('push', 'pop', 'ldm', 'stm', 'bx', 'b') for i in body):
        return None
    if any(i.mnemonic == 'b' or i.mnemonic.startswith('b') for i in body):
        return None
    # parameters: an argument register read before it is ever written
    written, nargs = set(), 0
    for i in body:
        try:
            rd, wr = i.regs_access()
            reads = {i.reg_name(x) for x in rd}
            writes = {i.reg_name(x) for x in wr}
        except Exception:
            reads, writes = set(), set()
        for k in range(4):
            r = f'r{k}'
            if r in reads and r not in written:
                nargs = max(nargs, k + 1)
        written |= writes
    return body, nargs


total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {q.stem for q in srcdir.glob('*.c')} if srcdir.exists() else set()
    cands = []
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size > max_size or size < 0x4:
            continue
        got = parse(addr, size)
        if got:
            cands.append((size, name, addr, got))
    n = min(limit, len(cands))
    print(f'[{mod}] intrinsic stubs: {len(cands)}, testing {n}', flush=True)
    hit = 0
    for size, name, addr, (body, nargs) in cands[:n]:
        # Data-processing immediates take '#'; coprocessor/status operands do not,
        # and CodeWarrior spells the status register `cpsr` where capstone says `apsr`.
        def text(strip_all=False):
            parts = []
            for i in body:
                op = i.op_str.replace('#', '') if (
                    strip_all or i.mnemonic in ('mcr', 'mrc', 'msr', 'mrs')) else i.op_str
                if i.mnemonic == 'mrs':
                    op = op.replace('apsr', 'cpsr')
                parts.append(f'{i.mnemonic} {op}')
            return '; '.join(parts)
        asm_hash = '; '.join(f'{i.mnemonic} {i.op_str}' for i in body)
        asm_plain = text()
        args = ', '.join(f'u32 a{k}' for k in range(nargs))
        variants = []
        for asm_text in (asm_plain, asm_hash, text(strip_all=True)):
            for sig in (f'void {name}({args})' if args else f'void {name}(void)',
                        f'void {name}(void)'):
                variants.append(f'{sig}\n{{\n    asm {{ {asm_text} }};\n}}\n')
        if t10.try_unit(name, addr, size, variants[0], variants=variants[1:], mod=mod):
            hit += 1
            print(f'  MATCH {name}', flush=True)
    print(f'[{mod}] asm matches: {hit}/{n}', flush=True)
    total += hit
print(f'total asm matches: {total}', flush=True)
