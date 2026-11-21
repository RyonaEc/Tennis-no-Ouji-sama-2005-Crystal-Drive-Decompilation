"""Synthesize tiny global-state accessors and test them against retail.

Usage: python build/fam.py ov002 <limit> [base_offset_hex]
"""
import importlib.util
import pathlib
import sys


def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m


T10 = load('build/t10.py', 't10')
A = load('build/auto.py', 'auto')

LD = {'ldrb': 'u8', 'ldrh': 'u16', 'ldr': 'u32'}
ST = {'strb': 'u8', 'strh': 'u16', 'str': 'u32'}


def imm(s):
    return int(s.split('#')[1], 16) if '#' in s else None


def dispoff(op):
    """offset inside 'ldr r0, [r1, #0x28]' -> 0x28; post/pre-index aware."""
    if '[' not in op:
        return None
    tail = op.split('[')[1]
    if '#' in tail:
        return int(tail.split('#')[1].split(']')[0], 16)
    return 0


def synth(name, ins, addr, size, deref, hdr):
    """Return list of candidate C sources."""
    out = []
    n = len(ins)
    mn = [i.mnemonic for i in ins]
    ops = [i.op_str for i in ins]

    # shape: data,deref,load r0,[r0,#off],bx   -> return field
    if n == 4 and mn[3] == 'bx' and mn[0] == 'ldr' and mn[1] == 'ldr' \
            and mn[2] in LD and ops[2].startswith('r0, [r0'):
        off = dispoff(ops[2]) or 0
        t = LD[mn[2]]
        out.append(f'{hdr}{t} {name}(void)\n{{\n'
                   f'    return *(volatile {t} *)({deref} + {hex(off)});\n}}\n'
                   .replace('volatile ', ''))

    # shape: data,deref,mov rX,#k,strX rX,[rY,#off],bx -> store constant
    if n == 5 and mn[4] == 'bx' and mn[3] in ST and mn[2] == 'mov':
        off = dispoff(ops[3]) or 0
        k = imm(ops[2]) or 0
        t = ST[mn[3]]
        out.append(f'{hdr}void {name}(void)\n{{\n'
                   f'    *(volatile {t} *)({deref} + {hex(off)}) = {k};\n}}\n'
                   .replace('volatile ', ''))

    # shape: data,deref,add r?,r?,#off,ldr/ldrh r0,[r?,#off2],bx
    if n == 5 and mn[4] == 'bx' and mn[3] in LD and mn[2] == 'add':
        o1 = imm(ops[2]) or 0
        o2 = dispoff(ops[3]) or 0
        t = LD[mn[3]]
        out.append(f'{hdr}{t} {name}(void)\n{{\n'
                   f'    return *(volatile {t} *)({deref} + {hex(o1 + o2)});\n}}\n'
                   .replace('volatile ', ''))

    # shape: data,deref,mov rX,#k,strX -> store constant with two adds
    if n == 6 and mn[5] == 'bx' and mn[4] in ST and mn[2] == 'mov' and mn[3] == 'add':
        o1 = imm(ops[3]) or 0
        o2 = dispoff(ops[4]) or 0
        k = imm(ops[2]) or 0
        t = ST[mn[4]]
        out.append(f'{hdr}void {name}(void)\n{{\n'
                   f'    *(volatile {t} *)({deref} + {hex(o1 + o2)}) = {k};\n}}\n'
                   .replace('volatile ', ''))

    # shape: data,deref,ldr/ldrb r?,[r?,#off],ands r0,..,movne/moveq,bx -> != 0
    if n >= 7 and mn[-1] == 'bx' and 'movne' in mn and 'ands' in mn:
        loads = [k for k in range(n) if mn[k] in LD and dispoff(ops[k]) is not None]
        if loads:
            k = loads[-1]
            off = dispoff(ops[k]) or 0
            adds = sum(imm(ops[j]) or 0 for j in range(n) if mn[j] == 'add')
            t = LD[mn[k]]
            out.append(f'{hdr}u32 {name}(void)\n{{\n'
                       f'    return (*(volatile {t} *)({deref} + {hex(off + adds)}) & 3) != 0;\n}}\n'
                       .replace('volatile ', ''))
    return out


def main(argv):
    mod = argv[0]
    limit = int(argv[1]) if len(argv) > 1 else 20
    base_off = int(argv[2], 16) if len(argv) > 2 else 0x2102070
    A.set_module(mod)
    base_sym = A.SYM[base_off]
    hdr = f'extern u8 {base_sym}[];\n'
    deref = f'*(u8 **){base_sym}'
    done = {q.stem for q in pathlib.Path(f'src/{mod}').glob('*.c')}
    cands = []
    for addr, (name, size) in A.FUNCS.items():
        if name in done or size > 0x40:
            continue
        ins = A.disasm(addr, size)
        if not any('[pc' in i.op_str and A._lit(i) == base_off for i in ins):
            continue
        cands.append((size, name, addr))
    cands.sort()
    print(f'candidates: {len(cands)}', flush=True)
    tried = kept = 0
    for size, name, addr in cands:
        if tried >= limit:
            break
        variants = synth(name, A.disasm(addr, size), addr, size, deref, hdr)
        if not variants:
            continue
        tried += 1
        ok = T10.try_unit(name, addr, size, variants[0],
                          variants=variants[1:], mod=mod)
        print(f'  {name} (0x{size:x}): {"KEPT" if ok else "miss"}', flush=True)
        kept += bool(ok)
    print(f'tried {tried}, kept {kept}', flush=True)


if __name__ == '__main__':
    main(sys.argv[1:])
