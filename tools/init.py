#!/usr/bin/env python3
"""Verifies the base ROM and initialises the dsd project configuration.

This performs the one-time setup:

1. Checks the ROM header (it must be the Japanese release, game code ATOJ).
2. Records the SHA-1 of your dump in config/ATOJ/tennis_ATOJ.sha1.
3. Runs ``dsd rom extract`` to split code and assets into extract/ATOJ/.
4. Runs ``dsd init`` to generate config/ATOJ/**/ (delinks, symbols, relocs).

The ROM and ARM7 BIOS are never committed; see docs/SETUP.md.

Usage:
    python tools/init.py [ROM] [--arm7-bios arm7_bios.bin]

The ROM may have any file name: pass it explicitly, or put a single .nds file
under extract/ and it is found automatically. The path is recorded in
config/ATOJ/rom.txt so the build does not need a fixed name either.

The dump may be encrypted or already decrypted. An encrypted one is detected
from its secure area and decrypted with an ARM7 BIOS, found at arm7_bios.bin,
bios7.bin or extract/arm7_bios.bin (--arm7-bios overrides). A decrypted dump
needs no BIOS.
"""

import argparse
import os
import shutil
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from rom_info import find_arm7_bios, secure_area_encrypted

ROOT = Path(__file__).resolve().parent.parent
GAME_ID = "ATOJ"
GAME = "tennis"
EXTRACT_DIR = ROOT / "extract" / GAME_ID
CONFIG_DIR = ROOT / "config"
BUILD_DIR = ROOT / "build"
EXPECTED_TITLE_FRAGMENT = "KCEJ"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def dumps() -> list[Path]:
    return sorted(p for p in (ROOT / "extract").glob("*.nds") if p.is_file())


def read_header(rom: Path) -> dict[str, object]:
    with rom.open("rb") as file:
        header = file.read(0x200)
    if len(header) < 0x200:
        raise SystemExit(f"{rom}: file is too small to be a DS ROM")

    title = header[0x00:0x0C].decode("ascii", "replace").rstrip("\x00 ")
    game_code = header[0x0C:0x10].decode("ascii", "replace")
    maker_code = header[0x10:0x12].decode("ascii", "replace")
    unit_code = header[0x12]
    version = header[0x1E]
    capacity = 128 * 1024 * (1 << header[0x14]) if header[0x14] else 0
    arm9_rom_offset, = struct.unpack_from("<I", header, 0x20)
    arm9_entry, = struct.unpack_from("<I", header, 0x24)
    arm9_ram_address, = struct.unpack_from("<I", header, 0x28)
    arm9_size, = struct.unpack_from("<I", header, 0x2C)

    return {
        "title": title,
        "game_code": game_code,
        "maker_code": maker_code,
        "unit_code": unit_code,
        "version": version,
        "capacity": capacity,
        "arm9_rom_offset": arm9_rom_offset,
        "arm9_entry": arm9_entry,
        "arm9_ram_address": arm9_ram_address,
        "arm9_size": arm9_size,
    }


def run(command: list[str], *, dry_run: bool) -> None:
    print("$ " + " ".join(command))
    if dry_run:
        return
    result = subprocess.run(command, cwd=ROOT)
    if result.returncode != 0:
        raise SystemExit(f"command failed with exit code {result.returncode}: {command[0]}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Initialise the project from a base ROM")
    parser.add_argument("rom", type=Path, nargs="?", help="Path to the base ROM (default: the only .nds under extract/)")
    parser.add_argument("--arm7-bios", type=Path, help="ARM7 BIOS dump, needed for encrypted ROMs")
    parser.add_argument("--dsd", type=Path, help="Path to the dsd CLI (default: ./dsd)")
    parser.add_argument("--dry-run", action="store_true", help="Print the commands without running them")
    args = parser.parse_args()

    rom: Path = args.rom
    if rom is None:
        found = dumps()
        if len(found) == 1:
            rom = found[0]
        elif not found:
            print("error: no .nds file under extract/", file=sys.stderr)
            print(
                "Put your own decrypted dump there (any file name works) or "
                "pass its path: python tools/init.py path/to/dump.nds",
                file=sys.stderr,
            )
            return 1
        else:
            print("error: more than one .nds file under extract/", file=sys.stderr)
            for path in found:
                print(f"  {rel(path)}", file=sys.stderr)
            print("Pass the one to use: python tools/init.py <path>", file=sys.stderr)
            return 1
    if not rom.is_file():
        print(f"error: no ROM at {rom}", file=sys.stderr)
        return 1
    rom = rom.resolve()

    header = read_header(rom)
    print(f"ROM:     {rom}")
    print(f"Title:   {header['title']}")
    print(f"Code:    {header['game_code']} ({header['maker_code']}, unit {header['unit_code']})")
    print(f"Version: {header['version']}")
    if header["capacity"]:
        print(f"Size:    {header['capacity'] // (1024 * 1024)} MiB ({header['capacity'] * 8 // (1024 * 1024)} Mbit)")
    print(
        "ARM9:    entry {:#010x}, RAM {:#010x}, {} bytes at ROM {:#x}".format(
            header["arm9_entry"],
            header["arm9_ram_address"],
            header["arm9_size"],
            header["arm9_rom_offset"],
        )
    )

    if header["game_code"] != GAME_ID:
        print(
            f"error: expected game code {GAME_ID} "
            "(Tennis no Ouji-sama 2005 Crystal Drive, Japan), "
            f"found {header['game_code']!r}",
            file=sys.stderr,
        )
        return 1
    encrypted = secure_area_encrypted(rom)
    bios = find_arm7_bios(ROOT, args.arm7_bios)
    if bios is not None:
        bios = bios.resolve()
    print("Secure area: " + ("encrypted" if encrypted else "decrypted"))
    if args.arm7_bios and bios is None:
        print(f"error: no ARM7 BIOS at {args.arm7_bios}", file=sys.stderr)
        return 1
    if encrypted and bios is None:
        print(
            "error: the secure area is encrypted and no ARM7 BIOS was found",
            file=sys.stderr,
        )
        print(
            "Put your own 16 KiB ARM7 BIOS dump at arm7_bios.bin (or bios7.bin, "
            "or pass --arm7-bios PATH).",
            file=sys.stderr,
        )
        print(
            "Dump it from your own console; see "
            "https://wiki.ds-homebrew.com/ds-index/ds-bios-firmware-dump",
            file=sys.stderr,
        )
        print(
            "It is only needed for encrypted dumps: a decrypted dump extracts "
            "without one.",
            file=sys.stderr,
        )
        return 1
    if encrypted:
        print(f"ARM7 BIOS:   {rel(bios)} (decrypts the secure area)")
    if EXPECTED_TITLE_FRAGMENT not in str(header["title"]).upper():
        print(
            f"warning: title {header['title']!r} does not look like Crystal Drive; continuing",
            file=sys.stderr,
        )

    dsd_exe = "dsd.exe" if os.name == "nt" else "dsd"
    if args.dsd:
        dsd = str(args.dsd)
    elif (ROOT / dsd_exe).is_file():
        dsd = str(ROOT / dsd_exe)
    else:
        dsd = dsd_exe
    if shutil.which(dsd) is None and not Path(dsd).is_file():
        print(f"error: dsd was not found ({dsd})", file=sys.stderr)
        print("Run `ninja download_tools`, or install dsd from", file=sys.stderr)
        print("https://github.com/AetiasHax/ds-decomp/releases", file=sys.stderr)
        return 1

    checksum_file = CONFIG_DIR / GAME_ID / f"{GAME}_{GAME_ID}.sha1"
    rom_record = CONFIG_DIR / GAME_ID / "rom.txt"
    if args.dry_run:
        print(f"\nWould write {rel(checksum_file)} with the SHA-1 of {rel(rom)}")
    else:
        sys.path.insert(0, str(ROOT / "tools"))
        from sha1 import sha1_of

        checksum_file.parent.mkdir(parents=True, exist_ok=True)
        rom_record.write_text(rel(rom) + "\n", encoding="utf-8")
        checksum_file.write_text(f"{sha1_of(rom)} {rom.name}\n", encoding="utf-8")
        print(f"\nRecorded SHA-1 in {rel(checksum_file)}")

    extract_command = [
        dsd,
        "rom",
        "extract",
        "--rom",
        rel(rom),
        "--output-path",
        rel(EXTRACT_DIR),
    ]
    if encrypted:
        extract_command += ["--arm7-bios", rel(bios)]
    run(extract_command, dry_run=args.dry_run)

    # `dsd init` rewrites delinks.txt/symbols.txt from scratch, which in this
    # repository would discard the delinks of every matched unit. The config is
    # versioned and only needs bootstrapping for a brand-new project, so keep it
    # whenever it is already there.
    arm9_config = CONFIG_DIR / GAME_ID / "arm9" / "config.yaml"
    if arm9_config.is_file():
        print(f"\nKeeping the existing project config at {rel(arm9_config.parent)} "
              "(dsd init would regenerate delinks.txt and lose the matched units)")
    else:
        run(
            [
                dsd,
                "init",
                "--rom-config",
                rel(EXTRACT_DIR / "config.yaml"),
                "--output-path",
                rel(CONFIG_DIR / GAME_ID),
                "--build-path",
                rel(BUILD_DIR / GAME_ID),
            ],
            dry_run=args.dry_run,
        )

    if not args.dry_run:
        if not arm9_config.is_file():
            print(f"error: dsd did not create {rel(arm9_config)}", file=sys.stderr)
            return 1
        print(f"\nProject initialised: {rel(arm9_config)}")
    print("\nNext: python tools/configure.py ATOJ && ninja")
    return 0


if __name__ == "__main__":
    sys.exit(main())
