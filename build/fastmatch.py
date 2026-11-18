"""Try candidate C for a function by compiling it alone and comparing bytes.

The full gate for a match is `t10.try_unit`, which links the whole ROM and
hashes it: a few seconds per attempt. Most attempts fail on the instruction
bytes long before that, so this compiles one translation unit with the same
flags the build uses and compares its `.text` against the retail bytes, which
takes about a tenth of a second. Only a byte-exact candidate is handed to
`t10.try_unit` to be confirmed and recorded.

Templates below are the shapes that have matched in this project so far; each
takes a function's disassembly and returns C, or None when the shape does not
apply. `scan` runs them over every unmatched function and reports the hits.

Usage:
    python build/fastmatch.py compile <file.c>          # byte-compare one file
    python build/fastmatch.py scan [--limit N] [mod ...] # template sweep
"""
import argparse
import importlib.util
import struct
import subprocess
import sys
import time
from pathlib import Path

MWCC_FLAGS = (
    "-O4,p -enum int -proc arm946e -gccext,on -fp soft -char signed "
    "-inline on,noauto -Cpp_exceptions off -gccinc -interworking -sym on "
    "-nolink -msgstyle gcc -str reuse -enc sjis -i include"
).split()


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


auto = load("build/auto.py", "auto")
t10 = load("build/t10.py", "t10")

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM  # noqa: E402

md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
md.detail = True


def text_of(obj: Path) -> bytes:
    """The .text bytes of a relocatable object."""
    data = obj.read_bytes()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    sections = [struct.unpack_from("<IIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    names_off = sections[shstrndx][4]

    def name_at(offset):
        end = data.index(b"\0", names_off + offset)
        return data[names_off + offset:end].decode()

    for name_off, _, _, _, offset, size in sections:
        if name_at(name_off) == ".text":
            return data[offset:offset + size]
    return b""


def relocs_of(obj: Path) -> list:
    """Offsets inside .text whose bytes are filled in by the linker."""
    data = obj.read_bytes()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    sections = [struct.unpack_from("<IIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    names_off = sections[shstrndx][4]

    def name_at(offset):
        end = data.index(b"\0", names_off + offset)
        return data[names_off + offset:end].decode()

    text_index = next((i for i, s in enumerate(sections) if name_at(s[0]) == ".text"), None)
    if text_index is None:
        return []
    # mwccarm emits .rela.text with 12-byte entries; plain .rel.text uses 8.
    for i, (_, _, _, _, offset, size) in enumerate(sections):
        name = name_at(sections[i][0])
        if name == ".rela.text":
            return [struct.unpack_from("<I", data, offset + k * 12)[0] for k in range(size // 12)]
        if name == ".rel.text":
            return [struct.unpack_from("<I", data, offset + k * 8)[0] for k in range(size // 8)]
    return []


def masked(blob: bytes, relocs) -> bytes:
    """Bytes with linker-filled words blanked, so objects with calls compare."""
    out = bytearray(blob)
    for offset in relocs:
        for k in range(offset, min(offset + 4, len(out))):
            out[k] = 0
    return bytes(out)


def compile_text(body: str, name: str) -> bytes:
    """Compile one function definition and return its code bytes."""
    src = Path("build/_fast.c")
    obj = Path("build/_fast.o")
    src.write_text('#include "types.h"\n\n' + body)
    try:
        result = subprocess.run(
            ["./tools/sjiswrap.exe", "tools/mwccarm/1.2/sp2/mwccarm.exe", *MWCC_FLAGS,
             "-c", str(src), "-o", str(obj)],
            capture_output=True, text=True,
        )
        if result.returncode:
            compile_text.last_relocs = []
            return b""
        compile_text.last_relocs = relocs_of(obj)
        return text_of(obj)
    finally:
        src.unlink(missing_ok=True)
        obj.unlink(missing_ok=True)


def compare(body: str, mod: str, addr: int, size: int) -> bool:
    """Is this body byte-exact for the function, ignoring linker-filled words?"""
    got = compile_text(body, "probe")
    want = retail(mod, addr, size)
    if not got:
        return False
    if got == want:
        return True
    relocs = compile_text.last_relocs
    return bool(relocs) and masked(got, relocs) == masked(want, relocs)


def retail(mod: str, addr: int, size: int) -> bytes:
    auto.set_module(mod)
    offset = addr - auto.BASE
    return auto.ROM[offset:offset + size]


def parse_ins(addr, size):
    return list(md.disasm(auto.ROM[addr - auto.BASE:addr - auto.BASE + size], addr))


def literal_of(ins):
    """The word a pc-relative load reads, and the symbol it names."""
    if "[pc" not in ins.op_str:
        return None
    token = [t for t in ins.op_str.replace("]", "").split(",") if "#" in t][-1]
    offset = int(token.split("#")[-1], 16)
    value = struct.unpack_from("<I", auto.ROM, ins.address - auto.BASE + 8 + offset)[0]
    return value


# --- templates ---------------------------------------------------------------

def two_call_wrapper(name, addr, size, ins):
    """`X(a, b); Y(a, b); return 0;` with both calls taking the arguments."""
    if len(ins) != 11:
        return None
    m = [i.mnemonic for i in ins]
    if m[0] != "push" or m[1] != "sub" or m[4] != "bl" or m[7] != "bl":
        return None
    if m[2:4] != ["mov", "mov"] or m[5:7] != ["mov", "mov"]:
        return None
    if ins[1].op_str.replace(" ", "") not in ("sp,sp,#0x4", "sp,sp,#4"):
        return None
    if ins[2].op_str.split(",")[1].strip() != "r0" or ins[3].op_str.split(",")[1].strip() != "r1":
        return None
    saved = (ins[2].op_str.split(",")[0].strip(), ins[3].op_str.split(",")[0].strip())
    if ins[5].op_str.replace(" ", "") != f"r0,{saved[0]}" or ins[6].op_str.replace(" ", "") != f"r1,{saved[1]}":
        return None
    if ins[8].op_str.replace(" ", "") != "r0,#0":
        return None
    callees = [auto.SYM.get(int(ins[i].op_str.split()[1], 16)) for i in (4, 7)]
    if not all(callees):
        return None
    return (f"s32 {name}(s32 a, s32 b)\n{{\n"
            f"    {callees[0]}(a, b);\n    {callees[1]}(a, b);\n    return 0;\n}}\n")


def callback_call(name, addr, size, ins):
    """`Fn fn = *(Fn *)*(u32 *)G; if (fn) fn();` through a global holder."""
    text = [f"{i.mnemonic} {i.op_str}" for i in ins]
    head = ["stmdb sp!, {lr}", "sub sp, sp, #4"]
    tail = ["add sp, sp, #4", "ldm sp!, {lr}", "bx lr"]
    if text[:2] != head or text[-3:] != tail:
        return None
    if [i.mnemonic for i in ins].count("blx") != 1 or "bxeq lr" not in text:
        return None
    holder = literal_of(ins[2])
    if holder is None or not auto.SYM.get(holder):
        return None
    return ("typedef void (*Fn)(void);\n"
            f"void {name}(void)\n{{\n"
            f"    Fn fn = *(Fn *)*(u32 *)0x{holder:08x};\n\n"
            "    if (!fn)\n        return;\n    fn();\n}\n")


def packed_halfword(name, addr, size, ins):
    """Null guard, then three shifted fields packed into the halfword at +4."""
    text = [f"{i.mnemonic} {i.op_str}" for i in ins]
    if text[:2] != ["cmp r0, #0", "bxeq lr"] or "orr" not in text:
        return None
    shifts = [t for t in text if t.startswith("lsl ") and ", #0x" in t]
    stores = [t for t in text if t.startswith("strh ")]
    if len(shifts) != 2 or len(stores) != 1:
        return None
    ands = [t for t in text if t.startswith("and ") and "#0x" in t]
    mask = None
    for i in ins:
        if i.mnemonic == "ldr" and "[pc" in i.op_str:
            mask = literal_of(i)
    if mask is None or len(ands) < 3:
        return None
    s1 = int(shifts[0].split("#")[-1], 16)
    s2 = int(shifts[1].split("#")[-1], 16)
    m1 = int(ands[0].split("#")[-1], 16)
    m2 = int(ands[1].split("#")[-1], 16)
    offset = int(stores[0].split("#")[-1].rstrip("]"), 16)
    return (f"void {name}(u8 *p, s32 a, s32 b, s32 c)\n{{\n"
            "    if (!p)\n        return;\n"
            f"    *(u16 *)(p + {offset:#x}) = ((a << {s1}) & {m1:#x})"
            f" | ((b << {s2}) & {m2:#x}) | (c & {mask:#x});\n}}\n")


TEMPLATES = (two_call_wrapper, callback_call, packed_halfword)


def candidates(mods):
    for mod in mods:
        auto.set_module(mod)
        src = Path(t10.srcdir_for(mod))
        have = {p.stem for p in src.glob("*.c")} if src.exists() else set()
        for addr, (name, size) in sorted(auto.FUNCS.items(), key=lambda kv: kv[1][1]):
            if name in have or size > 0x80 or size < 0x8:
                continue
            yield mod, name, addr, size


def scan(limit, mods, register):
    hit = tried = 0
    started = time.time()
    for mod, name, addr, size in candidates(mods):
        if tried >= limit:
            break
        try:
            ins = parse_ins(addr, size)
        except Exception:
            continue
        if not ins:
            continue
        for template in TEMPLATES:
            try:
                body = template(name, addr, size, ins)
            except Exception:
                body = None
            if not body:
                continue
            tried += 1
            if not compare(body, mod, addr, size):
                break
            print(f"  bytes {mod} {name} via {template.__name__}", flush=True)
            if register and t10.try_unit(name, addr, size, body, mod=mod):
                hit += 1
            break
    print(f"{hit} matched, {tried} candidates, {time.time() - started:.0f}s")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="cmd", required=True)
    one = sub.add_parser("compile", help="byte-compare one file against a function")
    one.add_argument("file")
    one.add_argument("--mod", default="arm9")
    one.add_argument("--addr", required=True, type=lambda v: int(v, 0))
    one.add_argument("--size", required=True, type=lambda v: int(v, 0))
    sweep = sub.add_parser("scan", help="apply the templates to unmatched functions")
    sweep.add_argument("--limit", type=int, default=400)
    sweep.add_argument("--register", action="store_true", help="keep byte-exact hits")
    sweep.add_argument("mods", nargs="*")
    args = parser.parse_args()

    if args.cmd == "compile":
        body = Path(args.file).read_text()
        got = compile_text(body, "probe")
        want = retail(args.mod, args.addr, args.size)
        if compare(body, args.mod, args.addr, args.size):
            print(f"byte-identical ({len(want)} bytes)")
            return 0
        if got == want:
            print(f"byte-identical ({len(got)} bytes)")
            return 0
        where = next((i for i in range(min(len(got), len(want))) if got[i] != want[i]), min(len(got), len(want)))
        print(f"{len(got)} bytes vs {len(want)}, first difference at {where:#x}")
        return 1

    mods = args.mods or ["arm9", "ov002", "ov003", "ov004", "ov005", "ov008", "ov009"]
    return scan(args.limit, mods, args.register)


if __name__ == "__main__":
    raise SystemExit(main())
