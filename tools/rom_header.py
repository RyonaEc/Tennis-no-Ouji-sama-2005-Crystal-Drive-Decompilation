"""Restore the header fields dsd's no-decrypt ROM build leaves stale.

`dsd rom build` computes the whole header when it re-encrypts the secure area
(the `--arm7-bios` path). When the base ROM is already decrypted there is
nothing to encrypt, and that path writes 0x0000 into the 16-bit secure-area
checksum at 0x6c instead of keeping retail's value.

That field is not derivable from the rebuilt ROM: it is the CRC16 (poly 0xA001,
init 0xFFFF) of the *encrypted* secure area, and a decrypted cartridge keeps the
checksum of the ciphertext it was made from. So it is carried over from the base
ROM header - metadata, like the banner and logo that dsd already copies - and the
header CRC at 0x15e, which covers it, is recomputed.

Usage: python tools/rom_header.py <rebuilt.nds> <base.nds>
       (or import `apply(built, base)` from tools/build_rom.py)
"""

import struct
import sys
from pathlib import Path

SECURE_AREA_CHECKSUM = 0x6C
HEADER_CRC = 0x15E
HEADER_CRC_END = HEADER_CRC


def crc16(data: bytes) -> int:
    """CRC16 with poly 0xA001 (reflected), init 0xFFFF - the header CRC."""
    c = 0xFFFF
    for byte in data:
        c ^= byte
        for _ in range(8):
            c = (c >> 1) ^ 0xA001 if c & 1 else c >> 1
    return c & 0xFFFF


def apply(built: Path, base: Path) -> int:
    """Carry the base ROM's 0x6c field into `built` and fix its header CRC."""
    if not built.is_file() or not base.is_file():
        print(f"error: missing {built} or {base}", file=sys.stderr)
        return 1

    header = bytearray(built.read_bytes()[:0x4000])
    base_header = base.read_bytes()[:0x4000]

    # Sanity: our CRC model must describe a real cartridge before we write with
    # it; otherwise the field layout changed and this step should not guess.
    if struct.unpack_from("<H", base_header, HEADER_CRC)[0] != crc16(base_header[:HEADER_CRC_END]):
        print(
            "error: base ROM header CRC at 0x15e does not match its own header; "
            "refusing to rewrite the rebuilt header",
            file=sys.stderr,
        )
        return 1

    checksum = base_header[SECURE_AREA_CHECKSUM : SECURE_AREA_CHECKSUM + 2]
    if header[SECURE_AREA_CHECKSUM : SECURE_AREA_CHECKSUM + 2] != checksum:
        header[SECURE_AREA_CHECKSUM : SECURE_AREA_CHECKSUM + 2] = checksum
        struct.pack_into("<H", header, HEADER_CRC, crc16(header[:HEADER_CRC_END]))
        with built.open("r+b") as file:
            file.write(header)
        print(
            "header: restored secure-area checksum "
            f"0x{struct.unpack_from('<H', header, SECURE_AREA_CHECKSUM)[0]:04x}, "
            f"recomputed header CRC 0x{struct.unpack_from('<H', header, HEADER_CRC)[0]:04x}"
        )
    return 0


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: python tools/rom_header.py <rebuilt.nds> <base.nds>", file=sys.stderr)
        return 1
    return apply(Path(sys.argv[1]), Path(sys.argv[2]))


if __name__ == "__main__":
    sys.exit(main())
