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
import json
import os
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
    # Per-process scratch names: a second agent compiling in the same tree
    # must not clobber this one.
    stem = f"build/_fast_{os.getpid()}"
    src = Path(stem + ".c")
    obj = Path(stem + ".o")
    src.write_text('#include "types.h"\n\n' + body)
    try:
        result = subprocess.run(
            ["./tools/sjiswrap.exe", "tools/mwccarm/1.2/b56/mwccarm.exe", *MWCC_FLAGS,
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


# --- compact iteration -------------------------------------------------------
# Writing the C is the work; reading listings and diffs is what burns context.
# These print the least needed to choose the next variant: one line per
# candidate, and a handful of differing instructions for the closest one.

def diff_summary(mod, addr, size, body):
    """(ok, compiled bytes, differing slots) for one candidate, compiled once."""
    blob = compile_text(body, "probe")
    want = retail(mod, addr, size)
    ok = False
    if blob:
        if blob == want:
            ok = True
        else:
            relocs = compile_text.last_relocs
            ok = bool(relocs) and masked(blob, relocs) == masked(want, relocs)
    want_ins = [(i.address - addr, i.mnemonic, i.op_str) for i in parse_ins(addr, size)]
    got_ins = [(i.address - addr, i.mnemonic, i.op_str) for i in md.disasm(blob, addr)] if blob else []
    diffs = []
    for i in range(max(len(want_ins), len(got_ins))):
        w = want_ins[i] if i < len(want_ins) else None
        g = got_ins[i] if i < len(got_ins) else None
        if w is not None and g is not None and w[1:] == g[1:]:
            continue
        diffs.append((w, g))
    return ok, len(blob), diffs


def _ins(t):
    return "(end)" if t is None else t[1] + " " + t[2]


def compact_lines(diffs, limit=8):
    out = []
    for w, g in diffs[:limit]:
        off = (w or g)[0]
        out.append("    %03x want %s | got %s" % (off, _ins(w), _ins(g)))
    if len(diffs) > limit:
        out.append("    ... %d more differing slots" % (len(diffs) - limit))
    return out


def run_diff(mod, addr, size, path, limit=12):
    ok, n, diffs = diff_summary(mod, addr, size, Path(path).read_text())
    if ok:
        print("MATCH %dB" % size)
        return 0
    print("%dB/%dB, %d differing slots" % (n, size, len(diffs)))
    print("\n".join(compact_lines(diffs, limit)))
    return 1


def run_variants(mod, addr, size, spec_path, limit=8):
    """Score many bodies from a JSON {"label": "body"} spec, one line each."""
    spec = json.loads(Path(spec_path).read_text())
    best = None
    for label, body in spec.items():
        ok, n, diffs = diff_summary(mod, addr, size, body)
        state = "MATCH" if ok else ("COMPILE-FAIL" if not n else "%dB/%dB" % (n, size))
        print("  %-20s %-9s %d diff" % (label, state, len(diffs)))
        if ok:
            print("WINNER " + label)
            return 0
        if best is None or len(diffs) < len(best[2]):
            best = (label, n, diffs)
    if best:
        print("closest %s (%dB/%dB):" % (best[0], best[1], size))
        print("\n".join(compact_lines(best[2], limit)))
    return 1


def wall_note(mod, addr):
    path = Path("build/walls.txt")
    if not path.exists():
        return None
    tag = "%s %#x " % (mod, addr)
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.startswith(tag):
            return line[len(tag):]
    return None


def run_brief(mod, addr, size):
    """A task card: enough to plan a body without dumping the listing."""
    auto.set_module(mod)
    name = auto.SYM.get(addr) or "func_%08x" % addr
    ins = parse_ins(addr, size)
    if not ins:
        print("%s %s %#x %dB: no instructions (stub or data)" % (mod, name, addr, size))
        return 0
    ops = {}
    for i in ins:
        ops[i.mnemonic] = ops.get(i.mnemonic, 0) + 1
    print("%s %s %#x %dB %d ins | wall: %s" % (mod, name, addr, size, len(ins), wall_note(mod, addr) or "none"))
    print("ops: " + " ".join("%s%d" % (k, v) for k, v in sorted(ops.items(), key=lambda kv: -kv[1])))
    calls = ["%03x:%s" % (i.address - addr, i.op_str.split()[-1]) for i in ins if i.mnemonic in ("bl", "blx")]
    if calls:
        print("calls: " + " ".join(calls[:8]))
    br = ["%03x %s" % (i.address - addr, i.op_str) for i in ins if i.mnemonic.startswith("b") and i.mnemonic not in ("bx", "bl", "blx")]
    if br:
        print("branches: " + " ".join(br[:10]))
    st = ["%03x %s" % (i.address - addr, i.mnemonic) for i in ins if i.mnemonic.startswith("str")]
    if st:
        print("stores: " + " ".join(st[:10]))
    lits = ["%#x" % literal_of(i) for i in ins if "[pc" in i.op_str and i.mnemonic in ("ldr", "ldrh")]
    if lits:
        print("pool: " + " ".join(lits[:10]))
    return 0


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
    d = sub.add_parser("diffc", help="compact diff of one candidate file")
    d.add_argument("mod")
    d.add_argument("addr", type=lambda v: int(v, 0))
    d.add_argument("size", type=lambda v: int(v, 0))
    d.add_argument("file")
    d.add_argument("--limit", type=int, default=12)
    vp = sub.add_parser("variants", help="score a JSON spec of candidate bodies")
    vp.add_argument("mod")
    vp.add_argument("addr", type=lambda v: int(v, 0))
    vp.add_argument("size", type=lambda v: int(v, 0))
    vp.add_argument("spec")
    vp.add_argument("--limit", type=int, default=8)
    bp = sub.add_parser("brief", help="task card for one function")
    bp.add_argument("mod")
    bp.add_argument("addr", type=lambda v: int(v, 0))
    bp.add_argument("size", type=lambda v: int(v, 0))
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

    if args.cmd == "diffc":
        return run_diff(args.mod, args.addr, args.size, args.file, args.limit)
    if args.cmd == "variants":
        return run_variants(args.mod, args.addr, args.size, args.spec, args.limit)
    if args.cmd == "brief":
        return run_brief(args.mod, args.addr, args.size)

    mods = args.mods or ["arm9", "ov002", "ov003", "ov004", "ov005", "ov008", "ov009"]
    return scan(args.limit, mods, args.register)


if __name__ == "__main__":
    raise SystemExit(main())
