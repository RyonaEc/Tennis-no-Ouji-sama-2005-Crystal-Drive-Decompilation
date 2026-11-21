# Setup

## ROMs and BIOS

This project never ships game data. You need:

* A decrypted dump of the Japanese cartridge (`NTR-ATOJ-JPN`, 64 MiB), placed
  anywhere under `extract/`. The file name does not matter: `tools/init.py`
  finds a single `.nds` there, or you pass the path explicitly.
* `arm7_bios.bin`, and only if your dump is encrypted. `dsd` uses it to decrypt
  secure-area blobs. Dump it from your own console; see the
  [DS homebrew wiki](https://wiki.ds-homebrew.com/ds-index/ds-bios-firmware-dump).

Do not commit either file; `.gitignore` covers both.

`tools/init.py` records the SHA-1 of the dump you use in
`config/ATOJ/tennis_ATOJ.sha1` and the path in `config/ATOJ/rom.txt`. A matching
build reproduces that exact revision, so keep both files in version control once
you have verified the rebuild.

## Toolchain

| Tool | Purpose | Obtained by |
|---|---|---|
| `dsd` | ROM extraction, delinking, LCF and ROM building | `ninja download_tools` (GitHub release) |
| `mwccarm` / `mwldarm` | CodeWarrior ARM compiler and linker | `ninja download_tools` (archive mirror) |
| `sjiswrap` | Passes Shift-JIS arguments to the compiler | `ninja download_tools` |
| `objdiff-cli` | Object diffing and progress reports | `ninja download_tools` |
| `wibo` | Runs the 32-bit Windows tools on Linux | `ninja download_tools` |

The CodeWarrior ARM compilers are vendored under `tools/mwccarm/` (one directory
per build, e.g. `1.2/sp2`), so a clean checkout configures without downloading
them. `ninja download_tools` only fetches mwccarm when the tree has none. If you
prefer an installation elsewhere, point at it with:

```shell
python tools/configure.py ATOJ --compiler /path/to/mwccarm
```

The directory must contain `<version>/mwccarm.exe`, for example
`tools/mwccarm/1.2/sp2/mwccarm.exe`. Select a service pack with
`--mwcc-version 2.0/sp2p2`.

### Per-unit compiler versions

Retail was not produced by a single mwccarm build: part of the game only
reproduces with the earlier `1.2/b56` build, while the default remains
`1.2/sp2`. Units that need a specific build are listed in
`config/ATOJ/mwcc_versions.txt` (one `<source path> = <version>` per line), and
`configure.py` gives those edges their own compiler. Keep both builds available:

```
tools/mwccarm/1.2/b56/mwccarm.exe
tools/mwccarm/1.2/sp2/mwccarm.exe
```

A version missing from the tree is a configure error, not a silent fallback. This
does not relax anything: a unit still only counts as matched when the linked ROM
hashes to `config/ATOJ/tennis_ATOJ.sha1`, so a unit compiled with the wrong
build fails the digest instead of passing approximately. Do not change the
default `--mwcc-version`, and do not add an entry to the map without that
byte-exact proof.

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

If the build says no base ROM was found, put your dump under `extract/` or pass
its path to `tools/init.py`; the recorded path lives in `config/ATOJ/rom.txt`.
`baserom_tennis_ATOJ.nds` is only a fallback name for older checkouts.

`error: expected game code ATOJ` means the image is a different region or a
different Prince of Tennis title.

If the compiler download fails, download the CodeWarrior archive by hand,
extract it so that `tools/mwccarm/1.2/sp2/mwccarm.exe` exists, and re-run
`configure.py`.

Link errors about `Entry` or missing symbols usually mean the linker script is
out of date. It is generated from `config/ATOJ/arm9/delinks.txt`, so re-run
`ninja delink` after editing delinks, and `ninja apply` after the ELF links so
symbol addresses stay in sync.
