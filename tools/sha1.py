#!/usr/bin/env python3
"""Computes or verifies a SHA-1 checksum.

Usage:
    python tools/sha1.py built.nds -c config/ATOJ/tennis_ATOJ.sha1
    python tools/sha1.py baserom.nds --write config/ATOJ/tennis_ATOJ.sha1
"""

import argparse
import hashlib
import sys
from pathlib import Path


def sha1_of(path: Path) -> str:
    digest = hashlib.sha1()
    with path.open("rb") as file:
        for chunk in iter(lambda: file.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description="Compute or verify a SHA-1 checksum")
    parser.add_argument("file", type=Path, help="File to hash")
    parser.add_argument("-c", "--checksum-file", type=Path, help="File containing the expected hash")
    parser.add_argument("--write", type=Path, help="Write '<sha1> <name>' to this file")
    args = parser.parse_args()

    file_hash = sha1_of(args.file)

    if args.write:
        args.write.parent.mkdir(parents=True, exist_ok=True)
        args.write.write_text(f"{file_hash} {args.file.name}\n", encoding="utf-8")
        print(f"{args.file}: wrote {file_hash} to {args.write}")
        return 0

    if args.checksum_file:
        target, _ = args.checksum_file.read_text(encoding="utf-8").split(" ", 1)
        target = target.strip()
        print(f"Target: {target}; {args.file.name}: {file_hash}")
        if target != file_hash:
            print(f"{args.file}: FAILED", file=sys.stderr)
            return 1
        print(f"{args.file}: OK", file=sys.stderr)
        return 0

    print(file_hash)
    return 0


if __name__ == "__main__":
    sys.exit(main())
