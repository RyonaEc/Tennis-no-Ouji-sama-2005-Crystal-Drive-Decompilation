"""Match the "clear/fill N slots at a struct field" loop family.

Retail shape:
    ldr  rS, [pc, #n]        ; address of the pointer slot
    mov  rI, #0              ; i = 0
    ldr  rB, [rS]            ; base = *slot
    mov  rV, #imm | mvn rV,#0 | mov rV,rI
  loop:
    add  rT, rB, rI[, lsl #2]
    add  rI, rI, #1
    strb/str rV, [rT, #off]
    cmp  rI, #count
    blt  loop

Associated as `*(T *)(base + i + off)`, which is what the compiler emits: the
base/index add is folded into the store's addressing mode, not the constant.
"""
import importlib.util, pathlib, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('build/auto.py', 'auto')
t10 = load('build/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 100
max_size = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x40
mods = sys.argv[3:] or ['arm9', 'ov002']


def parse(addr, size):
    ins = a.disasm(addr, size)
    if not ins or 'str' not in ' '.join(i.mnemonic for i in ins):
        return None
    slot = off = count = None
    value = None
    stride, width = 1, 'u8'
    for i in ins:
        if i.mnemonic == 'ldr' and '[pc' in i.op_str:
            try:
                slot = a.SYM.get(a._lit(i))
            except Exception:
                return None
        elif i.mnemonic in ('strb', 'str') and '#' in i.op_str and '[' in i.op_str:
            width = 'u8' if i.mnemonic == 'strb' else 'u32'
            try:
                off = int(i.op_str.rsplit('#', 1)[1].rstrip(']').strip(), 0)
            except ValueError:
                return None
        elif i.mnemonic == 'cmp' and i.op_str.split(',')[1].strip().startswith('#'):
            count = int(i.op_str.split(',')[1].strip()[1:], 0)
        elif i.mnemonic == 'mov' and ',' in i.op_str:
            dst, src = [x.strip() for x in i.op_str.split(',', 1)]
            if src.startswith('#'):
                value = int(src[1:], 0)
            elif src.startswith('r') and dst not in ('r13', 'r15'):
                value = 0            # mov rV, rI where rI was zeroed
        elif i.mnemonic == 'mvn':
            value = -1
        elif i.mnemonic == 'add' and 'lsl #2' in i.op_str:
            stride = 4
    if not slot or off is None or count is None or value is None:
        return None
    ct = 'u32' if width == 'u32' else 'u8'
    step = 'i * 4' if stride == 4 else 'i'
    v = str(value)
    head = f'extern u8 {slot}[];\n'
    pre = (f'    {ct} *base = *({ct} **){slot};\n'
           f'    s32 i;\n')
    loop = (f'    for (i = 0; i < {count}; i++) {{\n'
            f'        *({ct} *)(base + {step} + {off}) = {v};\n'
            f'    }}\n')
    text = head + f'void {{NAME}}(void)\n{{\n' + pre + loop + '}\n'
    alts = [
        text.replace('s32 i;', 'u32 i;'),
        text.replace(f'*({ct} *)(base + {step} + {off}) = {v};',
                     f'base[{off // stride} + i] = {v};'),
        text.replace(f'*({ct} *)(base + {step} + {off}) = {v};',
                     f'*({ct} *)(base + {step} + {hex(off)}) = {v};'),
    ]
    return text, alts


total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    have = {q.stem for q in srcdir.glob('*.c')} if srcdir.exists() else set()
    cands = []
    for addr, (name, size) in sorted(a.FUNCS.items()):
        if name in have or size > max_size or size < 0x8:
            continue
        got = parse(addr, size)
        if got:
            cands.append((size, name, addr, got))
    n = min(limit, len(cands))
    print(f'[{mod}] fill-family candidates: {len(cands)}, testing {n}', flush=True)
    hit = 0
    for size, name, addr, (text, alts) in cands[:n]:
        body = text.replace('{NAME}', name)
        vs = [x.replace('{NAME}', name) for x in alts]
        if t10.try_unit(name, addr, size, body, variants=vs, mod=mod):
            hit += 1
            print(f'  MATCH {name}', flush=True)
    print(f'[{mod}] matches: {hit}/{n}', flush=True)
    total += hit
print(f'total fill-family matches: {total}', flush=True)
