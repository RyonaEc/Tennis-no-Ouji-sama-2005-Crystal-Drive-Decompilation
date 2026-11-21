"""Compile one C unit with every CodeWarrior mwccarm build on disk and diff
each result against the retail bytes.

This is the "version sweep" from sm64ds-decomp: a residue that no source
spelling fixes is often a compiler-build delta, not a source problem.

usage: python tools/match/sweep.py <mod> <name>=<file.c> [versions...]
       (default: every tools/mwccarm/*/* that has an mwccarm.exe)
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
MD = Cs(CS_ARCH_ARM, CS_MODE_ARM)

FLAGS = ['-O4,p', '-enum', 'int', '-proc', 'arm946e', '-gccext,on',
         '-fp', 'soft', '-char', 'signed', '-inline', 'on,noauto',
         '-Cpp_exceptions', 'off', '-gccinc', '-interworking', '-sym', 'on',
         '-nolink', '-msgstyle', 'gcc', '-str', 'reuse', '-enc', 'sjis',
         '-i', 'include']


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
    out = set()
    p = pathlib.Path(f'config/ATOJ/{mod}/relocs.txt')
    if not p.exists():
        return out
    for line in p.read_text().splitlines():
        if not line.startswith('from:'):
            continue
        for field in line.split():
            if field.startswith('from:'):
                out.add(int(field[5:], 16))
    return out


def versions():
    out = []
    for p in sorted(pathlib.Path('tools/mwccarm').glob('*/*')):
        if (p / 'mwccarm.exe').exists():
            out.append(f'{p.parent.name}/{p.name}')
    return out


def compile_with(ver, cfile, obj):
    cc = ROOT / 'tools/mwccarm' / ver.replace('/', '/') / 'mwccarm.exe'
    wrap = ROOT / 'tools/sjiswrap.exe'
    argv = [str(wrap), str(cc)] + FLAGS + ['-c', str(cfile), '-o', str(obj)]
    if not wrap.exists():
        argv = [str(cc)] + FLAGS + ['-c', str(cfile), '-o', str(obj)]
    obj.unlink(missing_ok=True)
    r = subprocess.run(argv, capture_output=True, text=True)
    if r.returncode or not obj.exists():
        return None, (r.stdout + r.stderr).strip()[:300]
    return text_of(obj), None


def main(argv):
    mod, spec = argv[0], argv[1]
    wanted = argv[2:] or versions()
    A.set_module(mod)
    reloc = relocs_of(mod)
    name, cfile = spec.split('=', 1)
    addr = A.NAME2ADDR[name]
    size = A.FUNCS[addr][1]
    retail = bytes(A.ROM[addr - A.BASE:addr - A.BASE + size])
    rw = struct.unpack_from(f'<{size // 4}I', retail)

    tmp = ROOT / 'build/sweep_tmp'
    tmp.mkdir(parents=True, exist_ok=True)
    src = tmp / f'{name}.c'
    src.write_text('#include "types.h"\n\n' + pathlib.Path(cfile).read_text())

    print(f'== {name} (0x{size:x}) across {len(wanted)} compiler builds')
    best = None
    for ver in wanted:
        obj = tmp / f'{name}_{ver.replace("/", "_")}.o'
        buf, err = compile_with(ver, src, obj)
        if buf is None:
            print(f'   {ver:14s} compile failed: {err}')
            continue
        if len(buf) != size:
            print(f'   {ver:14s} size {len(buf)}B != {size}B')
            continue
        mw = struct.unpack_from(f'<{size // 4}I', buf)
        diffs = [k for k in range(size // 4)
                 if mw[k] != rw[k] and (addr + 4 * k) not in reloc]
        if not diffs:
            print(f'   {ver:14s} IDENTICAL  <<<')
            best = ver
        else:
            print(f'   {ver:14s} {len(diffs)} differ')
            if best is None and (best is None or len(diffs) < 99):
                pass
    print(f'   best: {best}' if best else '   no build reproduces this unit')
    for p in tmp.iterdir():
        p.unlink(missing_ok=True)
    tmp.rmdir()


if __name__ == '__main__':
    main(sys.argv[1:])
