"""Compile one C unit and diff its emitted .text against the retail bytes.

usage: python tools/match/cdiff.py <mod> <name>=<file.c> [more...]

Words that the linker fills from relocs.txt are shown as `~~` and are not
reported as differences: the pre-link object always holds 0 there.
"""
import importlib.util
import pathlib
import struct
import subprocess
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM


def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m


ROOT = pathlib.Path('.')
A = load('tools/match/auto.py', 'auto')
T = load('tools/match/t10.py', 't10')
MD = Cs(CS_ARCH_ARM, CS_MODE_ARM)


def text_of(path):
    d = pathlib.Path(path).read_bytes()
    if d[:4] != b'\x7fELF':
        return b''
    shoff = struct.unpack_from('<I', d, 0x20)[0]
    shent = struct.unpack_from('<H', d, 0x2e)[0]
    shnum = struct.unpack_from('<H', d, 0x30)[0]
    shstr = struct.unpack_from('<H', d, 0x32)[0]

    def sh(i):
        o = shoff + i * shent
        return struct.unpack_from('<II', d, o) + struct.unpack_from('<II', d, o + 0x10)

    _, _, stroff, _ = sh(shstr)
    for i in range(shnum):
        nm_off, _, off, size = sh(i)
        nm = d[stroff + nm_off:d.index(b'\x00', stroff + nm_off)].decode()
        if nm == '.text':
            return d[off:off + size]
    return b''


def relocs_of(mod):
    out = {}
    p = pathlib.Path(f'config/ATOJ/{mod}/relocs.txt')
    if not p.exists():
        return out
    for line in p.read_text().splitlines():
        if not line.startswith('from:'):
            continue
        want = None
        for field in line.split():
            if field.startswith('from:'):
                want = int(field[5:], 16)
        if want is not None:
            out[want] = line
    return out


def dis(buf, base):
    return [f'{i.mnemonic} {i.op_str}'.strip() for i in MD.disasm(buf, base)]


def main(argv):
    mod, specs = argv[0], argv[1:]
    A.set_module(mod)
    relocs = relocs_of(mod)
    for spec in specs:
        name, cfile = spec.split('=', 1)
        addr = A.NAME2ADDR[name]
        size = A.FUNCS[addr][1]
        srcdir = f'src/{mod}'
        src = ROOT / srcdir / f'{name}.c'
        dlmod = mod if mod == 'arm9' else f'arm9/overlays/{mod}'
        dl = ROOT / f'config/ATOJ/{dlmod}/delinks.txt'
        before = dl.read_text()
        src.write_text('#include "types.h"\n\n' + pathlib.Path(cfile).read_text())
        dl.write_text(before.rstrip('\n') + '\n\n' + T.entry_for(srcdir, name, addr, size))
        obj_path = ROOT / f'build/ATOJ/{srcdir}/{name}.o'
        obj_path.unlink(missing_ok=True)
        try:
            subprocess.run([sys.executable, 'tools/configure.py', 'ATOJ'], capture_output=True)
            r = subprocess.run([sys.executable, '-m', 'ninja', str(obj_path)],
                               capture_output=True, text=True)
            if r.returncode:
                lines = (r.stdout + r.stderr).strip().splitlines()
                print(f'--- {name}: COMPILE FAILED')
                print('\n'.join(l for l in lines
                                if 'error' in l.lower() or 'string not found' in l
                                or '.c:' in l)[:1500])
                continue
            mine = text_of(obj_path)[:size]
        finally:
            dl.write_text(before)
            src.unlink(missing_ok=True)
            subprocess.run([sys.executable, 'tools/configure.py', 'ATOJ'], capture_output=True)
        retail = bytes(A.ROM[addr - A.BASE:addr - A.BASE + size])
        mw = struct.unpack_from(f'<{len(mine) // 4}I', mine)
        rw = struct.unpack_from(f'<{size // 4}I', retail)
        diffs = [k for k in range(len(mw))
                 if mw[k] != rw[k] and (addr + 4 * k) not in relocs]
        ml, rl = dis(mine, addr), dis(retail, addr)
        tag = 'IDENTICAL' if not diffs else f'{len(diffs)} differ'
        print(f'--- {name}: {tag} (mine {len(mine)}B / retail {size}B)')
        if diffs:
            for k in range(max(len(ml), len(rl))):
                x = ml[k] if k < len(ml) else '<none>'
                y = rl[k] if k < len(rl) else '<none>'
                mark = '~~' if (addr + 4 * k) in relocs else ('!!' if x != y else '  ')
                print(f'   {addr + 4 * k:08x}  {mark} {x:30s} | {y}')


if __name__ == '__main__':
    main(sys.argv[1:])
