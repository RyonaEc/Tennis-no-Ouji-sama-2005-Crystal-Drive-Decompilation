"""Compile a generated unit in isolation and compare its bytes with retail.

Lets us check codegen equivalence without touching the linked ROM: the source is
built with the project's own mwcc flags and its .text is diffed against the
bytes at the retail address.  Literal-pool words that the final link patches are
masked out on both sides.
"""
import importlib.util
import pathlib
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
ARM9_BASE = 0x02004000
FLAGS = ("-O4,p -enum int -proc arm946e -gccext,on -fp soft -char signed -inline on,noauto "
         "-Cpp_exceptions off -gccinc -interworking -sym on -nolink -msgstyle gcc -str reuse "
         "-enc sjis -i include -lang=c -d ATOJ -c").split()
ZERO = b"\0"


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def sections(data):
    e_shoff, = struct.unpack_from("<I", data, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    hdrs = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        name, typ, flags, addr, offset, size = struct.unpack_from("<IIIIII", data, off)
        hdrs.append((name, typ, offset, size))
    strtab = data[hdrs[e_shstrndx][2]:hdrs[e_shstrndx][2] + hdrs[e_shstrndx][3]]
    named = {}
    for name_off, typ, offset, size in hdrs:
        end = strtab.index(ZERO, name_off)
        named[strtab[name_off:end].decode()] = (offset, size)
    return named


def reloc_offsets(data):
    named = sections(data)
    for sec, entsize in ((".rela.text", 12), (".rel.text", 8)):
        if sec in named:
            offset, size = named[sec]
            return {struct.unpack_from("<I", data, offset + i * entsize)[0]
                    for i in range(size // entsize)}
    return set()


def main(module, name, addr, size, body):
    out_dir = ROOT / "build" / "verify"
    out_dir.mkdir(parents=True, exist_ok=True)
    src = out_dir / f"{name}.c"
    obj = out_dir / f"{name}.o"
    src.write_text('#include "types.h"\n\n' + body, encoding="utf-8")
    cmd = [str(ROOT / "tools" / "sjiswrap.exe"),
           str(ROOT / "tools" / "mwccarm" / "1.2" / "sp2" / "mwccarm.exe")] + FLAGS + [str(src), "-o", str(obj)]
    res = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if res.returncode:
        return f"compile failed: {(res.stdout + res.stderr).strip()[:200]}"

    obj_data = obj.read_bytes()
    text_off, text_size = sections(obj_data)[".text"]
    got = obj_data[text_off:text_off + text_size]
    retail = (ROOT / "extract" / "ATOJ" / "arm9" / "arm9.bin").read_bytes()
    start = addr - ARM9_BASE
    want = retail[start:start + size]

    got_cmp = bytearray(got[:len(want)])
    want_cmp = bytearray(want)
    for off in reloc_offsets(obj_data):
        for i in range(off, min(off + 4, len(want_cmp))):
            if i < len(got_cmp):
                got_cmp[i] = want_cmp[i] = 0

    if bytes(got_cmp) == bytes(want_cmp):
        return f"bytes identical ({len(want)} bytes, {len(reloc_offsets(obj_data))} relocs masked)"
    n = min(len(got_cmp), len(want_cmp))
    diff = next((i for i in range(n) if got_cmp[i] != want_cmp[i]), n)
    return f"differs: {len(got)} vs {len(want)} bytes, first mismatch at +{diff}"


if __name__ == "__main__":
    auto = load(ROOT / "build" / "auto.py", "auto")
    module = sys.argv[1]
    auto.set_module(module)
    for name in sys.argv[2:]:
        addr = auto.NAME2ADDR[name]
        size = auto.FUNCS[addr][1]
        print(name, "->", main(module, name, addr, size, auto.emit(auto.gen(addr))))
