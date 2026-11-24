# Setup

## ROMs and BIOS

This project never ships game data. You need:

* A dump of the Japanese cartridge (`NTR-ATOJ-JPN`, 64 MiB), placed anywhere
  under `extract/`. The file name does not matter: `tools/init.py` finds a
  single `.nds` there, or you pass the path explicitly. An encrypted dump is
  accepted as it is.
* An ARM7 BIOS, only when your dump is encrypted. `tools/init.py` reads the
  secure area ID at `0x4000`: if it is still encrypted, the KEY1 key table from
  an ARM7 BIOS is needed to decrypt the first 2 KiB, and the same BIOS re-encrypts
  the rebuilt ROM so its digest matches. A decrypted dump needs no BIOS.

`init.py` looks for the BIOS at `arm7_bios.bin`, `bios7.bin` or
`extract/arm7_bios.bin`, and `--arm7-bios PATH` overrides that. It is the 16 KiB
ARM7 BIOS (`bios7.bin`, `BIOSNDS7.ROM`); dump it from your own console with
[`dsbf_dump`](https://github.com/DS-Homebrew/dsbf_dump), see the
[DS homebrew wiki](https://wiki.ds-homebrew.com/ds-index/ds-bios-firmware-dump).
If it is missing for an encrypted dump, `init.py` stops and says so rather than
extracting a garbled secure area.

**Which revision the digest pins.** `config/ATOJ/tennis_ATOJ.sha1` is
`9eec3248d3569eba8abcb3244adcbdf01a5ea424`: the cartridge image with the secure
area at `0x4000` **decrypted** - the form No-Intro catalogues and the usual
dumpers (`wooddumper`, `GodMode9i`) write by default. It needs no ARM7 BIOS.

The still-encrypted image of the same cartridge is
`8d555a80db47278475ffdfdf808fe80dbd7c48cf`, and the two differ only in
`0x4000`-`0x47ff`: the 2 KiB KEY1 secure-area block, which is the arm9's first
0x800 bytes (code starts at the entry point `0x02004800`, past it). The same
game, the same code, so matching is unaffected either way. If your dump is the
encrypted form, decrypt a copy (`ndstool -sd`, or NDecrypt without `-e`) and the
pinned digest matches.

**Why a build step fixes the header.** Decrypting changes only the secure-area
contents, but retail's header keeps the checksum of the *encrypted* block: the
16-bit field at `0x6c` is CRC16 (`0x4000`-`0x47ff` ciphertext, poly `0xA001`,
init `0xFFFF`) and the header CRC at `0x15e` covers it. `dsd rom build` writes
those only when it re-encrypts, so on a decrypted base ROM it leaves `0x6c` at
zero and the rebuild misses the digest by four bytes. `tools/build_rom.py` wraps
the build, carries the 16-bit field over from the base ROM header - metadata,
like the banner and logo dsd already copies - and recomputes the header CRC.
Those two header words are the only part of a rebuilt ROM not derived from source.

`tools/init.py` only runs `dsd init` when `config/ATOJ/arm9/config.yaml` is
missing, because that command regenerates `delinks.txt`/`symbols.txt` from
scratch and would discard every matched unit's placement. In a checkout where
the config already exists, `init.py` extracts the ROM, records the digest and
leaves the config alone - which is what makes a fresh clone build out of the box.

Do not commit the ROM or the BIOS; `.gitignore` covers both.

`tools/init.py` records the SHA-1 of the dump you use in
`config/ATOJ/tennis_ATOJ.sha1` and the path in `config/ATOJ/rom.txt`. A matching
build reproduces that exact revision, so keep both files in version control once
you have verified the rebuild.

## Toolchain

| Tool | Purpose | Obtained by |
|---|---|---|
| `dsd` | ROM extraction, delinking, LCF and ROM building | `ninja download_tools` (GitHub release) |
| `mwccarm` / `mwldarm` | CodeWarrior ARM compiler and linker | vendored under `tools/mwccarm/` |
| `sjiswrap` | Passes Shift-JIS arguments to the compiler | `ninja download_tools` |
| `objdiff-cli` | Object diffing and progress reports | `ninja download_tools` |
| `wibo` | Runs the 32-bit Windows tools on Linux | `ninja download_tools` |

The CodeWarrior ARM compilers are vendored under `tools/mwccarm/` (one directory
per build, e.g. `1.2/b56`), so a clean checkout configures without downloading
them. `ninja download_tools` only fetches mwccarm when the tree has none. If you
prefer an installation elsewhere, point at it with:

```shell
python tools/configure.py ATOJ --compiler /path/to/mwccarm
```

The directory must contain `<version>/mwccarm.exe`, for example
`tools/mwccarm/1.2/b56/mwccarm.exe`. Select another build with
`--mwcc-version <tag>`; the default is `1.2/b56`, the build retail used, so
changing it is a deliberate experiment and not a normal setup step.

### Per-unit compiler versions

Retail's game code was produced by a single mwccarm build: `1.2/b56` (build 56,
`Runtime Built: Sep 16 2004`, NITRO V1.0) rebuilds the whole ROM to the digest
for every unit, and that is `configure.py`'s default. The game shipped on
2004-12-30, so b56 (2004-09-16) is the only vendored build that predates the
cartridge and every later build postdates the finished ROM; a post-release build
is a prior against, never a proof against. The override list is therefore empty,
and it exists for a unit with evidence of a different origin; entries name an
existing build (one `<source path> = <version>` per line) and the digest still
has to match. The entry this file used to carry, `func_0201fcc8 = 2.0/base`, was
retired: the unit is NitroSDK's hand-written `MTX_RotX33_` asm, which b56
assembles byte for byte. Keep a build available if you add an entry for it:

```
tools/mwccarm/1.2/b56/mwccarm.exe
```

A version missing from the tree is a configure error, not a silent fallback. This
does not relax anything: a unit still only counts as matched when the linked ROM
hashes to `config/ATOJ/tennis_ATOJ.sha1`, so a unit compiled with the wrong
build fails the digest instead of passing approximately. Do not add an entry to
the map without that byte-exact proof, and do not change the default
`--mwcc-version` unless the digest still matches afterwards.

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
code is not `ATOJ`. A decrypted dump needs no ARM7 BIOS, and the tracked
`config/ATOJ` is kept as it is, so the three commands above are the whole
first build for someone whose dump matches the pinned revision.

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

If `configure.py` reports a missing compiler, check that the version directory
exists in your checkout (`tools/mwccarm/1.2/b56/mwccarm.exe`) and re-run it. To restore the directory,
`python tools/download_tool.py mwccarm latest --path tools/mwccarm` fetches the
archive mirror.

Link errors about `Entry` or missing symbols usually mean the linker script is
out of date. It is generated from `config/ATOJ/arm9/delinks.txt`, so re-run
`ninja delink` after editing delinks, and `ninja apply` after the ELF links so
symbol addresses stay in sync.
