#!/usr/bin/env python3
"""Rewrite Windows/Wine paths in a compiler dependency file.

The original CodeWarrior compiler runs under Wine or Wibo and emits ``.d``
files with Windows paths (``z:\\path\\to\\header.h``). Ninja needs host paths,
so the depfile is rewritten in place after every compile.

Usage:
    python tools/transform_dep.py build/ATOJ/src/foo.d
"""

import argparse
import os
import sys
from pathlib import Path


def wine_prefix() -> Path:
    return Path(os.environ.get("WINEPREFIX", Path.home() / ".wine"))


def convert(path: str) -> str:
    path = path.replace("\\", "/")
    if len(path) >= 2 and path[1] == ":":
        drive = path[0].lower()
        rest = path[2:]
        if drive == "z":
            return rest
        device = wine_prefix() / "dosdevices" / f"{drive}:"
        try:
            resolved = device.resolve()
        except OSError:
            return path
        return str(resolved) + rest
    return path


def transform(text: str) -> str:
    out: list[str] = []
    for index, line in enumerate(text.splitlines(keepends=True)):
        continued = line.endswith("\\\n")
        body = line.rstrip("\n")
        if continued:
            body = body[:-1]
        if index == 0:
            out.append(body.replace("\\", "/") + (" \\\n" if continued else "\n"))
            continue
        body = convert(body.strip())
        out.append("\t" + body + (" \\\n" if continued else "\n"))
    return "".join(out)


def main() -> int:
    parser = argparse.ArgumentParser(description="Transform Wine paths in a .d file")
    parser.add_argument("depfile", type=Path, help="Dependency file to rewrite in place")
    args = parser.parse_args()

    try:
        text = args.depfile.read_text(encoding="utf-8", errors="replace")
    except FileNotFoundError:
        print(f"warning: {args.depfile} was not produced", file=sys.stderr)
        return 0

    args.depfile.write_text(transform(text), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
