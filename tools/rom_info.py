#!/usr/bin/env python3
"""ROM header and ARM7 BIOS helpers shared by init.py and configure.py.

A cartridge dump may be encrypted or already decrypted. The secure area at
0x4000 starts with "encryObj" (or E7FFDEFF after the BIOS consumed it) when it
is decrypted, and with opaque bytes when it is encrypted. Decrypting it needs
the KEY1 table from an ARM7 BIOS at offset 0x30, so the BIOS is only required
when the dump is encrypted.
"""

import struct
from pathlib import Path

SECURE_AREA = 0x4000
DECRYPTED_IDS = (b"encryObj", b"\xe7\xff\xde\xff" * 2, b"\x00" * 8)
BIOS_NAMES = ("arm7_bios.bin", "bios7.bin", "extract/arm7_bios.bin")


def arm9_rom_offset(rom: Path) -> int:
    with rom.open("rb") as file:
        file.seek(0x20)
        return struct.unpack("<I", file.read(4))[0]


def has_secure_area(rom: Path) -> bool:
    return SECURE_AREA <= arm9_rom_offset(rom) < 0x8000


def secure_area_encrypted(rom: Path) -> bool:
    """True when the secure area still holds encrypted data."""
    if not has_secure_area(rom):
        return False
    with rom.open("rb") as file:
        file.seek(SECURE_AREA)
        return file.read(8) not in DECRYPTED_IDS


def find_arm7_bios(root: Path, explicit: Path | None = None) -> Path | None:
    """The ARM7 BIOS to decrypt with, or None.

    An explicit path wins; otherwise look next to the project and in extract/,
    which covers the names people usually dump it under.
    """
    if explicit is not None:
        return explicit if explicit.is_file() else None
    for name in BIOS_NAMES:
        path = root / name
        if path.is_file():
            return path
    return None
