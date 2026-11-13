#!/usr/bin/env python3
"""Downloads a prebuilt decompilation tool.

Usage:
    python tools/download_tool.py <tool> <tag> --path <path>

For archives the path is a directory, otherwise it is the output file.
"""

import argparse
import io
import stat
import sys
import urllib.request
import zipfile
from pathlib import Path

from get_platform import get_platform

MWCCARM_URL = "http://decomp.aetias.com/files/mwccarm.zip"


def build_url(tool: str, tag: str, platform) -> str:
    if tool == "dsd":
        return (
            "https://github.com/AetiasHax/ds-decomp/releases/download/"
            f"{tag}/dsd-{platform.system}-{platform.machine}{platform.exe}"
        )
    if tool == "wibo":
        return (
            "https://github.com/decompals/wibo/releases/download/"
            f"{tag}/wibo-{platform.machine}"
        )
    if tool == "sjiswrap":
        return (
            "https://github.com/encounter/sjiswrap/releases/download/"
            f"{tag}/sjiswrap-windows-x86.exe"
        )
    if tool == "objdiff":
        return (
            "https://github.com/encounter/objdiff/releases/download/"
            f"{tag}/objdiff-cli-{platform.system}-{platform.machine}{platform.exe}"
        )
    if tool == "mwccarm":
        return MWCCARM_URL
    raise SystemExit(f"Unknown tool: {tool}")


def download(url: str) -> bytes:
    request = urllib.request.Request(
        url, headers={"User-Agent": "crystal-drive-decomp/1.0"}
    )
    try:
        with urllib.request.urlopen(request, timeout=120) as response:
            return response.read()
    except Exception as error:  # noqa: BLE001 - surfaced to the user verbatim
        raise SystemExit(f"Failed to download {url}: {error}") from error


def main() -> int:
    parser = argparse.ArgumentParser(description="Download a prebuilt decompilation tool")
    parser.add_argument("tool", choices=["dsd", "wibo", "sjiswrap", "objdiff", "mwccarm"])
    parser.add_argument("tag", help="Release tag, or 'latest' for archives")
    parser.add_argument("--path", type=Path, required=True, help="Output file or directory")
    args = parser.parse_args()

    platform = get_platform()
    if platform is None:
        return 1

    url = build_url(args.tool, args.tag, platform)
    print(f"Downloading {args.tool} from {url}")
    data = download(url)

    if url.endswith(".zip"):
        args.path.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(io.BytesIO(data)) as archive:
            archive.extractall(args.path)
        print(f"Extracted to {args.path}")
    else:
        args.path.parent.mkdir(parents=True, exist_ok=True)
        args.path.write_bytes(data)
        args.path.chmod(args.path.stat().st_mode | stat.S_IEXEC | stat.S_IXGRP | stat.S_IXOTH)
        print(f"Wrote {args.path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
