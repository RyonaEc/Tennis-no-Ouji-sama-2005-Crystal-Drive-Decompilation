# Contributing

This is a **matching decompilation** of *Tennis no Ouji-sama 2005 Crystal Drive*
(`NTR-ATOJ-JPN`). Source in `src/` is compiled with the original CodeWarrior
toolchain (`mwccarm` 1.2/sp2) and linked back into the retail addresses. A
change is only progress when the rebuilt ROM is **byte-identical** to the
supported dump:

```
sha1: 8d555a80db47278475ffdfdf808fe80dbd7c48cf   (64 MiB)
```

Everything else — cleaner C, fewer functions, "almost matching" objects — is a
regression. Please read this page before opening a pull request; the rules below
are what keeps the build reproducible for everyone.

## Ground rules

* **No game data in the repository.** Do not commit the ROM, the extracted
  `extract/` tree, `arm7_bios.bin`, assets ripped from the cartridge, or the
  proprietary `tools/mwccarm/` compilers. `.gitignore` covers them; see
  [docs/SETUP.md](docs/SETUP.md) for supplying your own dump.
* **No approximate matches.** Do not stub callees, drop behaviour, delete
  branches or hand-write assembly to make a unit's size line up. A unit is either
  matching C or it stays delinked assembly.
* **No weakened verification.** Do not edit `tools/sha1.py`, the expected SHA-1
  in `config/ATOJ/tennis_ATOJ.sha1`, the objdiff configuration, or any check to
  turn a failure into a pass.
* **No compiler changes to force a match.** Compiler version and `CC_FLAGS` in
  `tools/configure.py` mirror the retail build. If a source shape resists the
  compiler, the source shape is wrong, not the flags.
* **No unrelated changes.** Do not reformat files, rename existing symbols
  (`func_*`, `data_*` names are referenced by `symbols.txt`, `delinks.txt` and
  the objdiff configuration), upgrade tooling or touch delinked assembly in a
  pull request that claims to match functions.

## Getting a build

Follow [README.md](README.md#setup) and [docs/SETUP.md](docs/SETUP.md):

```shell
python -m pip install ninja
# put your decrypted dump at extract/baserom_tennis_ATOJ.nds
python tools/init.py
python tools/configure.py ATOJ
ninja
```

Before writing any C, confirm the delinked build already reproduces the dump:

```shell
ninja check    # per-module comparison against the base ROM
ninja sha1     # whole-ROM digest
```

Both must pass on a clean checkout. If they do not, the problem is your setup
(usually the dump, the compiler version or an out-of-date `delinks.txt`), not a
function you have not written yet.

## What counts as a contribution

1. A function's body is written as C in `src/<module>/<name>.c` — one delinked
   unit per file, usually a single function — using the name the delinks entry
   does (`src/arm9/func_02023358.c`, `src/ov003/func_ov003_02066de0.c`).
2. The matching delinks entry (`config/ATOJ/arm9/delinks.txt`, or
   `config/ATOJ/arm9/overlays/<module>/delinks.txt` for an overlay) is present and
   marked complete, and any `symbols.txt` change the unit needs is included.
3. `ninja check` reports the module as matching **and** `ninja sha1` prints
   `OK` against `8d555a80db47278475ffdfdf808fe80dbd7c48cf`.
4. `README.md` and `PROGRESS.md` are consistent with the new totals if the
   headline counts moved. `PROGRESS.md` is generated from the objdiff report
   (`ninja report`); never hand-edit it.
5. The commit contains only the sources, delinks/symbols entries and progress
   numbers for the batch.

A unit that diffs "close" is not done. Tools that compare disassembly with
relocation bytes masked can report a match while the linked ROM still differs,
because constant-pool contents and pc-relative displacements are part of the
image. The ROM digest is the only gate.

## Writing the C

* `#include "types.h"` is the first line of every unit; the shared typedefs
  (`u8`, `u16`, `u32`, `s8`, `s16`, `s32`, `u64`, `BOOL`) live in
  [include/types.h](include/types.h). Without the include the compiler emits
  `<string not found>` and the unit fails.
* Declare what a unit calls as `extern` with the retail signature; call
  overlay-local functions by their overlay name (`func_ov009_0204deec`, not
  `func_0204deec`), or the link fails.
* Keep the delinked address ranges intact. A translation unit covers exactly the
  range in `delinks.txt`; do not move code across units.
* Function and data names come from `symbols.txt`. If a symbol is still unnamed,
  keep the address-based name rather than inventing a descriptive one.

Getting `mwccarm` to emit retail's instruction sequence is the actual work.
[notes/mwccarm-codegen.md](notes/mwccarm-codegen.md) is the project's codegen
playbook — branch versus predication, magic-number division, `u16` counters,
address materialisation, register-allocation walls — and
[docs/WORKFLOW.md](docs/WORKFLOW.md) describes the delink/compile/link loop.
When a function cannot be reached from any source shape you have tried, leave it
delinked and record the evidence instead of committing an approximation.

## Submitting

* Branch from `main`, keep the change focused, and open a pull request that
  names the functions matched (module, `func_*` name, size).
* State in the pull request what you ran: `ninja check`, `ninja sha1`, and the
  digest line that proves it. Do not claim a match you did not verify.
* Commit messages describe the batch and why the shape works, in the imperative
  mood, e.g. `Match two replay-state helpers and two running-sum loops.` Group a
  batch into one commit; do not add AI attribution, `Co-Authored-By` trailers or
  bot co-authors.
* Do not force-push `main` or rewrite published history. The maintainer keeps
  `main` as a single linear series of batches, so a merged pull request may be
  squashed.
* Pull requests that weaken a check, add non-matching code, or include
  copyrighted binaries will be closed.

## Reporting problems

Open an issue for build or tooling problems. Include your OS, the output of
`python tools/configure.py ATOJ`, and the exact failing command. Never attach
the ROM, BIOS dumps, extracted game files or compiler archives to an issue.

For AI agents working in this repository, [AGENTS.md](AGENTS.md) restates the
same rules as a step-by-step loop, including the local matching helpers under
`build/`.
