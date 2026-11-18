# Setup

## ROMs and BIOS

This project never ships game data. You need:

* `extract/baserom_tennis_ATOJ.nds`, a decrypted dump of the Japanese cartridge
  (`NTR-ATOJ-JPN`, 64 MiB).
* `arm7_bios.bin`, and only if your dump is encrypted. `dsd` uses it to decrypt
  secure-area blobs. Dump it from your own console; see the
  [DS homebrew wiki](https://wiki.ds-homebrew.com/ds-index/ds-bios-firmware-dump).

Do not commit either file; `.gitignore` covers both.

`tools/init.py` records the SHA-1 of the dump you use in
`config/ATOJ/tennis_ATOJ.sha1`. A matching build reproduces that exact revision,
so keep the file in version control once you have verified the rebuild.

## Toolchain

| Tool | Purpose | Obtained by |
|---|---|---|
| `dsd` | ROM extraction, delinking, LCF and ROM building | `ninja download_tools` (GitHub release) |
| `mwccarm` / `mwldarm` | CodeWarrior ARM compiler and linker | `ninja download_tools` (archive mirror) |
| `sjiswrap` | Passes Shift-JIS arguments to the compiler | `ninja download_tools` |
| `objdiff-cli` | Object diffing and progress reports | `ninja download_tools` |
| `wibo` | Runs the 32-bit Windows tools on Linux | `ninja download_tools` |

`mwccarm` is proprietary and is not part of this repository. It is fetched into
`tools/mwccarm/` on first use so the project builds on a clean checkout. If you
already have a CodeWarrior installation, point at it with:

```shell
python tools/configure.py ATOJ --compiler /path/to/mwccarm
```

The directory must contain `<version>/mwccarm.exe`, for example
`tools/mwccarm/1.2/sp2/mwccarm.exe`. Select a service pack with
`--mwcc-version 2.0/sp2p2`.

On Windows the compilers run natively. On Linux, `configure.py` runs them
through Wibo by default; pass `-w /path/to/wine` to use Wine instead.

## First build

```shell
python -m pip install ninja
python tools/init.py
python tools/configure.py ATOJ
ninja
```

`init.py` prints the ROM header it found and refuses to continue if the game
code is not `ATOJ`.

If `dsd` is missing, the first `configure.py` run emits a build file whose only
target is `download_tools`:

```shell
ninja            # downloads the tools
python tools/configure.py ATOJ
ninja
```

## Troubleshooting

If `extract/baserom_tennis_ATOJ.nds` is missing, copy your dump to exactly that
path. The name comes from `GAME` and the game code in `tools/configure.py`.

`error: expected game code ATOJ` means the image is a different region or a
different Prince of Tennis title.

If the compiler download fails, download the CodeWarrior archive by hand,
extract it so that `tools/mwccarm/1.2/sp2/mwccarm.exe` exists, and re-run
`configure.py`.

Link errors about `Entry` or missing symbols usually mean the linker script is
out of date. It is generated from `config/ATOJ/arm9/delinks.txt`, so re-run
`ninja delink` after editing delinks, and `ninja apply` after the ELF links so
symbol addresses stay in sync.
