"""Print the register or stack slot mwcc gave every parameter and local.

`-g` does not change .text (verified), so a candidate can be compiled with
debug info and the DWARF read back: this answers "which variable is in r4"
instead of guessing from declaration order.

Usage: python tools/match/slots.py <file.c> [function]
"""
import importlib.util
import pathlib
import struct
import subprocess
import sys

from elftools.elf.elffile import ELFFile


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
MWCC = ["./tools/sjiswrap.exe", "tools/mwccarm/1.2/sp2/mwccarm.exe", *fm.MWCC_FLAGS]

REGS = {0x50 + i: f"r{i}" for i in range(16)}


def sleb(data, i):
    result = shift = 0
    while True:
        byte = data[i]
        i += 1
        result |= (byte & 0x7F) << shift
        shift += 7
        if not byte & 0x80:
            if byte & 0x40:
                result -= 1 << shift
            return result, i


def describe(expr):
    """DWARF location expression -> 'rN' / '[fb+0xNN]' / raw hex."""
    if not expr:
        return "-"
    op = expr[0]
    if op in REGS:
        return REGS[op]
    if op == 0x91:                     # DW_OP_fbreg
        value, _ = sleb(expr, 1)
        return f"[fb{value:+#x}]"
    if op == 0x92 and expr[1] in REGS:  # DW_OP_bregN + offset
        value, _ = sleb(expr, 2)
        return f"{REGS[expr[1]]}{value:+#x}"
    if op == 0x03:                     # DW_OP_addr
        return "addr"
    return f"expr:{expr.hex()}"


def main():
    src = pathlib.Path(sys.argv[1])
    wanted = sys.argv[2] if len(sys.argv) > 2 else None
    obj = pathlib.Path("build/cand/_slots.o")
    proc = subprocess.run([*MWCC, "-g", "-o", str(obj), str(src)],
                          capture_output=True, text=True)
    if proc.returncode:
        print((proc.stdout + proc.stderr)[:800])
        return 1

    with obj.open("rb") as f:
        elf = ELFFile(f)
        if not elf.has_dwarf_info():
            print("no debug info in object")
            return 1
        dwarf = elf.get_dwarf_info(relocate_dwarf_sections=False)
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            for die in top.iter_children():
                if die.tag != "DW_TAG_subprogram":
                    continue
                name = die.attributes.get("DW_AT_name")
                if name is None:
                    continue
                func = name.value.decode() if isinstance(name.value, bytes) else name.value
                if wanted and func != wanted:
                    continue
                rows = []
                for child in die.iter_children():
                    if child.tag not in ("DW_TAG_formal_parameter", "DW_TAG_variable"):
                        continue
                    var = child.attributes.get("DW_AT_name")
                    loc = child.attributes.get("DW_AT_location")
                    if var is None:
                        continue
                    var = var.value.decode() if isinstance(var.value, bytes) else var.value
                    where = "-"
                    if loc is not None:
                        try:
                            expr = dwarf.get_DIE_from_refaddr(loc.value + cu.cu_offset) \
                                if isinstance(loc.value, int) else loc.value
                        except Exception:
                            expr = loc.value
                        if isinstance(expr, bytes):
                            where = describe(expr)
                        elif hasattr(expr, "loc_expr"):
                            where = describe(expr.loc_expr)
                        else:
                            where = describe(expr) if expr else "-"
                    rows.append((var, where))
                if rows:
                    print(f"=== {func} ===")
                    for var, where in rows:
                        print(f"  {var:24s} {where}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
