"""Template cloning.

Two functions with the same instruction skeleton (same mnemonics and register
allocation, differing only in immediates and the symbols their literal pools
reference) must have the same C up to those constants and names. This tool
finds unmatched functions whose skeleton equals a matched function's, clones
the matched source, substitutes the constants/symbols, and tests the result.

usage: python build/clone.py [mod] [maxsize] [--test] [--limit N]
"""
import importlib.util
import pathlib
import re
import sys


def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m


A = load('build/auto.py', 'auto')
T = load('build/t10.py', 't10')

SYMRE = re.compile(r'\b(?:data|func)_[0-9a-f]{8}\b')
NUMRE = re.compile(r'0x[0-9a-fA-F]+|\b\d+\b')


LOOSE = False


def profile(mod, addr, size):
    """(skeleton, [symbols], [immediates]) for one function."""
    skel, syms, imms = [], [], []
    for i in A.disasm(addr, size):
        parts = [i.mnemonic]
        for o in i.operands:
            if o.type == 1:                  # register
                parts.append('R' if LOOSE else 'r%d' % o.reg)
            elif o.type == 2:                # immediate
                parts.append('#')
            elif o.type == 3:                # memory
                m = o.mem
                if LOOSE:
                    parts.append('[]')
                else:
                    parts.append('[%d+%d]' % (m.base, m.index or 0))
            else:
                parts.append('?')
        skel.append(','.join(parts))
        # symbols / immediates, in program order
        if i.mnemonic.startswith('bl') or i.mnemonic == 'b':
            for o in i.operands:
                if o.type == 2:
                    nm = A.SYM.get(o.imm)
                    if nm:
                        syms.append(nm)
                    else:
                        imms.append(o.imm)
            continue
        if '[pc' in i.op_str:
            try:
                v = A._lit(i)
            except Exception:
                continue
            nm = A.SYM.get(v)
            if nm:
                syms.append(nm)
            else:
                imms.append(v)
            continue
        for o in i.operands:
            if o.type == 2:
                imms.append(o.imm)
    return tuple(skel), syms, imms


def clone_with(src, old_name, new_name, symmap, nummap):
    """Rewrite a matched source for the new function."""
    out = src
    # protect symbol identifiers so their digits are not touched
    for k, old in enumerate(sorted(symmap, key=len, reverse=True)):
        out = re.sub(r'\b%s\b' % re.escape(old), '\x01%d\x01' % k, out)
    def numsub(m):
        tok = m.group(0)
        v = int(tok, 0)
        if v in nummap:
            return '0x%x' % nummap[v]
        return tok
    out = NUMRE.sub(numsub, out)
    for k, old in enumerate(sorted(symmap, key=len, reverse=True)):
        out = out.replace('\x01%d\x01' % k, symmap[old])
    out = re.sub(r'\b%s\b' % re.escape(old_name), new_name, out)
    return out


def main(argv):
    mod = argv[0] if argv else 'arm9'
    maxsize = int(argv[1], 0) if len(argv) > 1 else 0x80
    do_test = '--test' in argv
    global LOOSE
    LOOSE = '--loose' in argv
    limit = 60
    if '--limit' in argv:
        limit = int(argv[argv.index('--limit') + 1], 0)
    A.set_module(mod)
    srcdir = pathlib.Path('src/%s' % mod)
    done = {q.stem for q in srcdir.glob('*.c')}

    # index matched functions by skeleton
    index = {}
    for q in sorted(srcdir.glob('*.c')):
        name = q.stem
        addr = A.NAME2ADDR.get(name)
        if addr is None:
            continue
        size = A.FUNCS[addr][1]
        try:
            skel, syms, imms = profile(mod, addr, size)
        except Exception:
            continue
        index.setdefault(skel, []).append((name, syms, imms, q.read_text()))

    cands = []
    for addr, (name, size) in sorted(A.FUNCS.items()):
        if name in done or size > maxsize:
            continue
        try:
            skel, syms, imms = profile(mod, addr, size)
        except Exception:
            continue
        for mname, msyms, mimms, msrc in index.get(skel, []):
            if len(syms) != len(msyms) or len(imms) != len(mimms):
                continue
            smap, clash = {}, False
            for old, new in zip(msyms, syms):
                if smap.get(old, new) != new:
                    clash = True
                    break
                smap[old] = new
            nsz = len(set(syms))
            if clash and nsz > 1:
                continue
            nmap, clash = {}, False
            for old, new in zip(mimms, imms):
                if nmap.get(old, new) != new:
                    clash = True
                    break
                nmap[old] = new
            if clash:
                continue
            src = clone_with(msrc, mname, name, smap, nmap)
            cands.append((name, addr, size, src, mname))
            break

    print('%s: %d clone candidates (<=0x%x)' % (mod, len(cands), maxsize))
    hit = 0
    for name, addr, size, src, mname in cands[:limit]:
        if not do_test:
            print('--- %s (0x%x)  <- %s' % (name, size, mname))
            continue
        ok = T.try_unit(name, addr, size, src, mod=mod)
        print('   -> %s <- %s: %s' % (name, mname, 'KEPT' if ok else 'miss'), flush=True)
        hit += 1 if ok else 0
    if do_test:
        print('%s: kept %d of %d tested' % (mod, hit, min(limit, len(cands))))


if __name__ == '__main__':
    main(sys.argv[1:])
