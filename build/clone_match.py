"""Match unmatched functions that are structural clones of an already-matched unit.

Two functions with the same instruction sequence (registers kept, every immediate
wildcarded) differ only in their constants, so the matched unit's C is a template:
rename the function, then swap the data symbols and call targets in order of
appearance.  The SHA-1 gate rejects any template that turns out not to fit.
"""
import importlib.util, pathlib, re, sys

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('build/auto.py', 'auto')
t10 = load('build/t10.py', 't10')

limit = int(sys.argv[1]) if len(sys.argv) > 1 else 20
mods = sys.argv[2:] or ['arm9', 'ov002']

IMM = re.compile(r'#-?(0x[0-9a-f]+|\d+)')


def key(ins_list):
    out = []
    for i in ins_list:
        op = IMM.sub('#I', i.op_str.replace(' ', ''))
        out.append((i.mnemonic, op))
    return tuple(out)


def externals(addr, size):
    """Names this function references, in order of appearance."""
    ins = a.disasm(addr, size)
    data, calls = [], []
    for i in ins:
        if i.mnemonic in ('ldr', 'ldrh', 'ldrb', 'ldrsh', 'ldrsb') and '[pc' in i.op_str:
            try:
                nm = a.SYM.get(a._lit(i))
            except Exception:
                nm = None
            if nm:
                data.append(nm)
        elif i.mnemonic in ('bl', 'b') and i.op_str.startswith('#'):
            try:
                tgt = int(i.op_str[1:], 16)
            except ValueError:
                continue
            nm = a.SYM.get(tgt)
            if nm and nm.startswith('func_'):
                calls.append(nm)
    return data, calls


total = 0
for mod in mods:
    a.set_module(mod)
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    matched, unmatched = {}, {}
    for addr, (name, size) in a.FUNCS.items():
        try:
            k = key(a.disasm(addr, size))
        except Exception:
            continue
        if (srcdir / f'{name}.c').exists():
            matched.setdefault(k, (name, addr, size))
        else:
            unmatched.setdefault(k, []).append((name, addr, size))
    pairs = []
    for k, (mname, maddr, msize) in matched.items():
        for uname, uaddr, usize in unmatched.get(k, []):
            if usize == msize:
                pairs.append((usize, mname, uname, uaddr, usize))
    pairs.sort()
    n = min(limit, len(pairs))
    print(f'[{mod}] structural clones of matched units: {len(pairs)}, testing {n}', flush=True)
    hit = 0
    for usize, mname, uname, uaddr, use in pairs[:n]:
        text = (srcdir / f'{mname}.c').read_text()
        md, mc = externals(maddr, msize := a.FUNCS[maddr][1])
        ud, uc = externals(uaddr, use)
        if len(md) != len(ud) or len(mc) != len(uc):
            continue
        body = re.sub(rf'\b{mname}\b', uname, text)
        for old, new in zip(md + mc, ud + uc):
            body = re.sub(rf'\b{old}\b', new, body)
        if t10.try_unit(uname, uaddr, use, body, mod=mod):
            hit += 1
            print(f'  MATCH {uname} (clone of {mname})', flush=True)
    print(f'[{mod}] clone matches: {hit}/{n}', flush=True)
    total += hit
print(f'total clone matches: {total}', flush=True)
