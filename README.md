<p align="center">
  <img src="assets/logo.png" width="760" alt="Tennis no Ouji-sama 2005 CRYSTAL DRIVE">
</p>

<h1 align="center">Tennis no Ouji-sama 2005 CRYSTAL DRIVE Decompilation</h1>

[![Code](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation.svg?mode=shield&label=Code%20matched&measure=matched_code_percent)](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation)
[![Functions](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation.svg?mode=shield&label=Functions&measure=matched_functions)](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation)

A work-in-progress matching decompilation of *テニスの王子様 2005 CRYSTAL DRIVE*
(Nintendo DS, Japan), built on [`ds-decomp`](https://github.com/AetiasHax/ds-decomp).

The goal is source that compiles with the original CodeWarrior toolchain into a
ROM that matches the retail release byte for byte.

| | |
|---|---|
| Title | テニスの王子様 2005 CRYSTAL DRIVE |
| Game code | `NTR-ATOJ-JPN` (`ATOJ`) |
| Publisher / developer | Konami / KCEJ |
| Release | 2004-12-30 |
| Cartridge | 512 Mbit (64 MiB) |
| Target | ARM9 (ARM946E-S), ARM7, overlays |
| Supported dump | `sha1: 9eec3248d3569eba8abcb3244adcbdf01a5ea424` (64 MiB) |

## Status

The toolchain works end to end: the delinked build links and rebuilds a
byte-for-byte identical ROM, including all eleven ARM9 overlays.

```
$ ninja sha1
Target: 9eec3248d3569eba8abcb3244adcbdf01a5ea424; tennis_ATOJ.nds: 9eec3248d3569eba8abcb3244adcbdf01a5ea424
tennis_ATOJ.nds: OK
```

| | Matching C | Total | Share |
|---|---:|---:|---:|
| Functions | 1,843 | 3,806 | 48.42% |
| Code | 160,712 B | 1,841,700 B | 8.73% |
| Data | 630,724 B | 801,936 B | 78.65% |
| Units | 4,132 | 4,785 | 86.35% |

The badges above use objdiff's stricter pair, `matched_functions` and `matched_code`, which
now agree with the complete counts exactly: every one of the 4,132 units scores 100%, so a
complete unit and a matched unit are the same thing today.

Data is now matched through the same pipeline as code: a delinks entry with a `.data`
range and a `.c` file that declares the array links at the original address with the
digest unchanged, and objdiff scores it (`data_02042240`, 36 B). Adding a source needs a
`python tools/configure.py ATOJ` re-run because `build.ninja` only lists sources known at
configure time.

The remaining 1,963 functions are still delinked assembly. Complete units compile with
`mwccarm` 1.2/b56 - build 56, `Runtime Built: Sep 16 2004`, the single build retail used -
link at the original addresses, and leave the ROM digest unchanged. [PROGRESS.md](PROGRESS.md) has the fuller breakdown - code,
functions, fuzzy match, data and a per-module table - and `python tools/match/progress.py`
regenerates it.

Everything below needs your own dump of the cartridge. No ROM or BIOS image is
included in this repository.

## Setup

### Prerequisites

* Python 3.10+
* [Ninja](https://ninja-build.org/) (`python -m pip install ninja`)
* Windows, or Linux with [Wibo](https://github.com/decompals/wibo) (downloaded automatically)

The 32-bit CodeWarrior compilers are vendored under `tools/mwccarm/`, including
`1.2/b56`, the build retail used, so a clean checkout builds without downloading
them. See [docs/SETUP.md](docs/SETUP.md) if you
would rather use an installation elsewhere.

### 1. Provide the base ROM

Put your dump anywhere under `extract/` - the file name does not matter, and an
encrypted dump is fine - or pass its path explicitly:

```shell
python tools/init.py "extract/Tennis no Ouji-sama 2005 CRYSTAL DRIVE.nds"
```

`init.py` checks the secure area: an encrypted dump is decrypted with your
`arm7_bios.bin` (also looked for as `bios7.bin` or `extract/arm7_bios.bin`), and
an already-decrypted dump needs no BIOS at all. See
[docs/SETUP.md](docs/SETUP.md#roms-and-bios).

### 2. Extract and initialise

```shell
python tools/init.py
```

This verifies the ROM header, records your dump's SHA-1 in
`config/ATOJ/tennis_ATOJ.sha1`, extracts code and assets with `dsd rom extract`,
and generates `config/ATOJ/` with delinks, symbols and relocations.

### 3. Configure and build

```shell
python tools/configure.py ATOJ
ninja
```

`ninja` builds `tennis_ATOJ.nds` from the delinked objects. At this stage the
result should already match the base ROM, which proves the toolchain and link
layout before any C is written.

## Targets

| Command | Result |
|---|---|
| `ninja rom` | Link `build/ATOJ/arm9.o` and build `tennis_ATOJ.nds` |
| `ninja check` | Verify every module against the base ROM (`dsd check`) |
| `ninja sha1` | Compare the built ROM's SHA-1 with `config/ATOJ/tennis_ATOJ.sha1` |
| `ninja objdiff` | Generate `objdiff.json` for [objdiff](https://github.com/encounter/objdiff) |
| `ninja report` | Per-function progress report (`build/ATOJ/report.json`) |
| `ninja delink` | Re-delink the base ROM after editing `delinks.txt` |
| `ninja dis` | Disassemble the base ROM into `build/ATOJ/asm/` |
| `ninja apply` | Copy symbol data from the linked ELF back into `symbols.txt` |
| `ninja download_tools` | Fetch `dsd`, `sjiswrap`, `objdiff` (and `wibo`); `mwccarm` only if the tree has none |

## Layout

```
config/ATOJ/        dsd configuration: per-module delinks, symbols, relocs
src/                decompiled C/C++ (one translation unit per delinked file)
include/            shared headers
libs/               vendored libraries (headers are added to the include path)
tools/              extraction, build and matching tooling, vendored `mwccarm`
extract/            your ROM and its extraction (never committed)
build/ATOJ/         delinked objects, linker script, EFX and the built ROM
build/              untracked scratch: candidates, logs, probes
```

## Matching notes

[`ROADMAP.md`](ROADMAP.md) is where the remaining work is and in what order.
[`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md) is the project's codegen
playbook: a short symptom-to-lever table for the source shapes that make
`mwccarm` emit which instructions on this title (addressing modes, register
colouring, branch versus predication, signedness), the workflow that gates a
match, and the classes that are not reachable from C. The dated evidence behind
it - measured diffs, bodies tried and per-family histories - is kept apart in
[`notes/mwccarm-codegen-log.md`](notes/mwccarm-codegen-log.md).

If you are an AI agent contributing to this repository, read
[`AGENTS.md`](AGENTS.md) first: it states what counts as a finished unit, the
hard rules that keep the ROM matching, the edit loop and the matching tools in
`tools/match/`, and how to commit and publish a batch.

## Legal

This project reverse-engineers the game for preservation and research. The
source code, build scripts and tooling in this repository are released under the
[MIT license](LICENSE). It ships no game code, assets, ROMs or BIOS images; you
must supply your own legally obtained dump. The CodeWarrior compilers vendored
under `tools/mwccarm/` are third-party binaries included because the build needs
the exact versions retail used, and they remain under their own terms.
*Tennis no Ouji-sama* and all related marks are property of their respective
owners.
