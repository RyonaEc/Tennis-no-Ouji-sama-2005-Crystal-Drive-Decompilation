#!/usr/bin/env python3
"""Relax the alignment of linked code sections to a halfword.

Retail code contains functions that start at 4n+2 addresses.  mwcc emits
`.text` with sh_addralign 4, and dsd emits its gap objects the same way, so the
linker pads such a chunk up to the next word and every byte after it shifts by
two.  The delinked sizes already match retail exactly, so alignment is the only
thing that decides placement: dropping it to 2 lets the linker pack the chunks
back to back, which is retail's layout.

The section headers are patched in place, which is idempotent and leaves data
and read-only sections alone.  Only touching files that change keeps ninja from
relinking for nothing.

Usage:
    python tools/align_code.py --stamp build/ATOJ/code_aligned.stamp \
        build/ATOJ/delinks build/ATOJ/src
"""

import struct
import sys
from pathlib import Path

SHT_PROGBITS = 1
SHF_EXECINSTR = 0x4
SH_ADDRAILGN = 32


def relax(path: Path) -> int:
    """Set every executable progbits section to sh_addralign 2."""
    data = bytearray(path.read_bytes())
    if data[:4] != b"\x7fELF" or data[4] != 1:
        return 0
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, _ = struct.unpack_from("<HHH", data, 0x2E)
    if shentsize < 40 or shoff + shnum * shentsize > len(data):
        return 0
    changed = 0
    for index in range(shnum):
        base = shoff + index * shentsize
        sh_type, sh_flags = struct.unpack_from("<II", data, base + 4)
        align, = struct.unpack_from("<I", data, base + SH_ADDRAILGN)
        if sh_type == SHT_PROGBITS and sh_flags & SHF_EXECINSTR and align > 2:
            struct.pack_into("<I", data, base + SH_ADDRAILGN, 2)
            changed += 1
    if changed:
        path.write_bytes(data)
    return changed


def relax_lcf(path: Path) -> bool:
    """Drop the ARM9 output section's ALIGNALL from 4 to 2.

    mwld's `ALIGNALL(4)` overrides the alignment the objects declare, so the
    section headers have to be relaxed together with it.  The directive is left
    alone in the data sections, where words have to stay aligned.
    """
    text = path.read_text(encoding="utf-8")
    start = text.find(".arm9 : {")
    if start < 0:
        return False
    index = text.find("ALIGNALL(4);", start)
    if index < 0:
        return False
    patched = text[:index] + "ALIGNALL(2);" + text[index + len("ALIGNALL(4);"):]
    if patched == text:
        return False
    path.write_text(patched, encoding="utf-8")
    return True


def main(argv: list[str]) -> int:
    stamp = None
    lcf = None
    dirs = []
    args = list(argv)
    while args:
        arg = args.pop(0)
        if arg == "--stamp":
            stamp = Path(args.pop(0))
        elif arg == "--lcf":
            lcf = Path(args.pop(0))
        else:
            dirs.append(Path(arg))

    seen = patched = sections = 0
    for directory in dirs:
        if not directory.is_dir():
            continue
        for path in sorted(directory.rglob("*.o")):
            seen += 1
            count = relax(path)
            if count:
                patched += 1
                sections += count

    print(f"align: {sections} code sections in {patched}/{seen} objects relaxed")
    if lcf is not None and lcf.is_file():
        print(f"align: linker script packed at 2 bytes: {relax_lcf(lcf)}")
    if stamp is not None:
        stamp.parent.mkdir(parents=True, exist_ok=True)
        stamp.write_text(f"{patched}\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
