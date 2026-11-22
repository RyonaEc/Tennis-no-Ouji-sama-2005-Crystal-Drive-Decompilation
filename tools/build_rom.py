"""Run `dsd rom build`, then restore the header fields it leaves stale.

Ninja invokes rules without a shell on this setup, so the two steps cannot be
chained with `&&` in build.ninja. This wrapper takes `--base <bench reference>`
for itself and passes everything else to dsd unchanged:

    python tools/build_rom.py <dsd> [dsd flags] rom build --config C --rom OUT \
        --base <base.nds> [--arm7-bios P]

See tools/rom_header.py for what the extra step repairs and why it is only a
header-metadata carry-over from the verified base ROM.
"""

import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import rom_header  # noqa: E402


def main(argv: list[str]) -> int:
    if len(argv) < 2 or "--base" not in argv:
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 1
    dsd = argv[0]
    args = argv[1:]
    i = args.index("--base")
    base = Path(args[i + 1])
    dsd_args = args[:i] + args[i + 2 :]

    result = subprocess.run([dsd, *dsd_args])
    if result.returncode:
        return result.returncode

    rom = Path(dsd_args[dsd_args.index("--rom") + 1])
    return rom_header.apply(rom, base)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
