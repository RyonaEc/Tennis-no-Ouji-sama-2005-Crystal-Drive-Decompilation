#!/usr/bin/env python3
"""Generates objdiff.json from dsd, shaped for ByteMatch.

dsd names its units after the objects it delinks (``_dsd_gap@main_3``).
ByteMatch groups units by the "/" segments of a unit's name, so every module
is mapped to its place in the source tree here. Units that already come from a
source file keep the name and paths dsd gives them.

Usage:
    python tools/objdiff_gen.py <dsd> <config.yaml> [dsd objdiff args...]
"""

import json
import re
import subprocess
import sys
from pathlib import Path

MODULES = {
    "main": "arm9/main",
    "itcm": "arm9/itcm",
    "dtcm": "arm9/dtcm",
}

_GAP_UNIT = re.compile(r"_dsd_gap@([A-Za-z0-9]+)_\d+")


def unit_name(name: str) -> str:
    match = _GAP_UNIT.fullmatch(name)
    if match is None:
        return name
    module = match.group(1)
    if module.startswith("ov"):
        return f"arm9/overlays/{module}"
    return MODULES.get(module, f"arm9/{module}")


def source_path(unit: dict) -> str:
    """The C file a unit is built from, derived from its object path.

    dsd already writes ``metadata.source_path`` for units it compiles from
    source; this is the same answer for a dsd that leaves it out, so ByteMatch
    can open a unit without searching the repository. ``build/ATOJ/src/arm9/x.o``
    becomes ``src/arm9/x.c``.
    """
    base = unit.get("base_path", "")
    marker = "src/"
    index = base.find(marker)
    if index < 0:
        return ""
    return base[index:][: -len(".o")] + ".c"


def shape(data: dict) -> dict:
    for unit in data.get("units", []):
        unit["name"] = unit_name(unit["name"])
        metadata = unit.setdefault("metadata", {})
        metadata.setdefault("complete", False)
        if "base_path" not in unit:
            # Still delinked assembly: there is no source to link to yet.
            metadata.pop("source_path", None)
            continue
        if not metadata.get("source_path"):
            derived = source_path(unit)
            if derived:
                metadata["source_path"] = derived
    return data


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: objdiff_gen.py <dsd> <config.yaml> [args...]", file=sys.stderr)
        return 2

    dsd, config, *extra = sys.argv[1:]
    result = subprocess.run(
        [dsd, "--force-color", "objdiff", "--config-path", config, "--stdout", *extra],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(result.stderr.strip(), file=sys.stderr)
        return 1

    data = shape(json.loads(result.stdout))
    Path("objdiff.json").write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
