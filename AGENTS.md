# Working on this repository (notes for AI agents)

This is a **matching decompilation** of *Tennis no Ouji-sama 2005 Crystal Drive*
(NTR-ATOJ-JPN, Nintendo DS). The only thing that counts as progress is source
that compiles with the original CodeWarrior toolchain into a ROM that is
**byte-identical** to the retail dump.

Read this before touching anything. Everything below is either a hard rule or a
proven shortcut; ignoring the hard rules breaks the build for everyone.

## Definition of done

A function is matched only when **all** of these hold:

1. `python tools/match/t10.py --check <module>` reports `ok` for that module
   (`dangling=0`, `unlinked=0`, `dupes=0`); without the local tooling, `ninja
   check` must pass for that module instead, and
2. `ninja sha1` prints
   `Target: 9eec3248d3569eba8abcb3244adcbdf01a5ea424; tennis_ATOJ.nds: 9eec3248d3569eba8abcb3244adcbdf01a5ea424`
   followed by `tennis_ATOJ.nds: OK`.

A unit whose object diff looks close is **not** done. `tools/match/var3.py` compares
disassembly with relocation bytes masked; it can say `MATCH` while the linked
ROM still differs (constant pools, pc-relative displacements, literal values).
Never commit a unit on the strength of `var3` alone.

## Hard rules

- **Never accept a degenerate or approximate unit.** No "close enough", no
  removing functionality to make sizes line up, no stubbing a callee.
- **Never generalise inline assembly into an ordinary C function.** Delinked
  assembly units stay assembly until the real C is found.
- **Never change compiler flags, the compiler version or `tools/configure.py`
  defaults** to force a match. If a source shape resists, the source is wrong.
  The default is `1.2/b56` because that is measured fact: build 56 (Runtime
  Built 2004-09-16) reproduces the full digest with all 1,536 matched units and
  **zero** per-file overrides, where the old `1.2/sp2` default needed 72 of them.
  A unit may still name a different pre-existing build in
  `config/ATOJ/mwcc_versions.txt`, but only with the same evidence: the full ROM
  digest must still match. Never guess a version; the sha1 gate, not the object
  diff, decides.
- **Never weaken verification.** Do not edit `tools/match/t10.py`, `tools/sha1.py`,
  the expected SHA-1, or the objdiff configuration to make a check pass. Do not
  delete tests or skip modules in `--check`.
- **Never commit the ROM or BIOS dumps** — `.gitignore` covers them and this
  project must stay free of copyrighted game binaries. The CodeWarrior ARM
  compilers under `tools/mwccarm/` *are* tracked: the build needs the exact
  versions retail used, so they are vendored.
- **Do not kill processes by pattern** (`pkill -f`, `killall`). A long build may
  be running; kill the exact PID you started, or nothing at all.

## Layout

```
extract/ATOJ/            extracted modules (arm9.bin, overlays, files) - not in git
config/ATOJ/arm9/        arm9 delinks.txt, symbols.txt, relocs.txt (plus itcm/, dtcm/)
config/ATOJ/arm9/overlays/<ov>/   the same three files for each overlay
src/<module>/*.c         decompiled translation units, usually one function per file
include/                 shared types (types.h is included by every unit)
build/ATOJ/delinks/      relocatable objects produced by dsd delink
build/ATOJ/report.json   objdiff's per-unit report, consumed by tools/match/progress.py
tools/                   dsd/mwccarm wrappers, configure.py, sha1.py, upload_report.py
tools/match/             matching tools, harness and rosters (see its README)
docs/                    build documentation (SETUP.md, WORKFLOW.md, BYTEMATCH.md)
```

Modules are `arm9` and the overlays `ov000` ... `ov010`; `config/ATOJ/arm9/`
also holds `itcm/` and `dtcm/`. Function names are
`func_<address>` (`func_020237f8`, `func_ov003_02066de0`); data symbols follow
`data_<address>`. Keep those names — `symbols.txt`, `delinks.txt` and the
objdiff configuration all key off them.

## The edit loop

1. Pick an unmatched function. `tools/match/famsurvey.py` groups unmatched functions
   by size and mnemonic sequence; groups of identical skeleton are the cheapest wins because one
   source shape covers every member.
2. Disassemble it: `python tools/match/udis.py <module> 0xADDR [0xADDR ...]`.
3. Write the C in a scratch file (e.g. `build/cand/name.c`) and compare with
   `python tools/match/var3.py <module> 0xADDR <size> build/cand/name.c`.
4. When `var3` says `MATCH`, install it with
   `python tools/match/reg.py <module> <func_name> 0xADDR <size> build/cand/name.c`.
   `reg.py` adds the delinks entry, rebuilds the ROM and only keeps the unit if
   the digest still matches; on a miss it restores the tree.
5. Re-verify: `python tools/match/t10.py --check <module>` and `ninja sha1`.
6. Regenerate `PROGRESS.md` (`rm -f build/ATOJ/report.json && python tools/match/progress.py`),
   update the counts in `README.md`, and commit source plus
   `config/.../delinks.txt` together.

Source files must `#include "types.h"` first, or the compiler emits
`<string not found>` and the unit fails to compile.

## Helper tools

| Command | Purpose |
|---|---|
| `python tools/match/udis.py <mod> <addr>...` | disassemble retail functions with sizes |
| `python tools/match/var3.py <mod> <addr> <size> <file.c>` | compile a candidate and diff it against retail |
| `python tools/match/reg.py <mod> <name> <addr> <size> <file.c>` | install a candidate, rebuild, keep only on ROM match |
| `python tools/match/t10.py --check <mod>...` | self-check delinks/source bookkeeping |
| `python tools/match/t10.py --repair <mod>` | drop dangling entries, relink orphan sources, drop duplicates |
| `python tools/match/autorun.py <limit> <maxsize> <mod>...` | try auto-generated candidates (`tools/match/tried.txt` remembers them) |
| `python tools/match/famsurvey.py <mod>...` | list unmatched identical-skeleton groups |
| `python tools/match/progress.py` | regenerate `PROGRESS.md` from objdiff's report |
| `python tools/match/names.py --dry-run` | report the scripted rename pass from `notes/naming/annotations.tsv` (names are recorded, not applied: see `docs/NAMING.md`) |
| `ninja report` / `ninja sha1` / `ninja check` | objdiff numbers, ROM digest, per-module check |
| `M:\melonDS-workbench` (melonDS decomp workbench) | runtime evidence: did the unit run, who calls it through a pointer, what arguments, who writes a field, which ROM file a buffer came from; setup and limits in `notes/workbench.md` |

`tools/match/fastmatch.py` is the shared compiler harness: `parse_ins(addr, size)`,
`retail(mod, addr, size)`, `compile_text(source, name)`, `literal_of(ins)`.
Literal-pool words are at `ins.address + 8 + imm12`.

The matching tools live in `tools/match/`, which has its own README:
`fastmatch.py` (harness), `vprobe.py` (all compilers in one run), `udis.py`,
`var3.py`, `reg.py`, `t10.py`, `autorun.py`, `famsurvey.py`, `permute.py`,
`progress.py` and the analysis scripts around them. `build/` is output only -
delinked objects and the built ROM (`build/ATOJ/`), scratch candidates
(`build/cand/`), logs and probes - and is gitignored in full.
`tools/match/true_fresh.txt` is the trusted unmatched queue.

## Runtime evidence (melonDS workbench)

`M:\melonDS-workbench` is a melonDS fork that records coverage, call edges,
write history and data provenance, and serves them over a local JSON-RPC API.
It answers what the digest cannot: did this unit ever run, who calls it through
a pointer, what arguments a call site sets up, who writes a struct field, and
which ROM file a RAM buffer came from. It never decides a match - `ninja sha1`
still does, and an observed call graph says nothing about byte equality. It
records the digest-pinned decrypted ROM, so its records are evidence about the
code we rebuild. Setup, API recipes, the overlay table and what it cannot do:
`notes/workbench.md`.

## Compiler idioms worth knowing

`notes/mwccarm-codegen.md` is the project's codegen playbook (a short lever
table); `notes/mwccarm-codegen-log.md` holds the dated evidence behind it. Read
the playbook before spending iterations on a stubborn unit, and the log when you
need to know whether a shape has already been tried.

- **Magic-number division.** `smull` + `asr` + sign fixup with
  `0x66666667` (shift 1) is `/5`; `0xb60b60b7` with `add hi, hi, n, lsl #8` then
  `asr #8` is `/360`; `0x157c`-style literals are plain source constants.
  Write the division and let mwcc pick the sequence.
- **u16 counters** produce `add rX, rX, #1 / lsl #0x10 / lsr rX, #0x10`.
  Use a `u16` loop variable to get that; a `u32` will not.
- **Branch layout is chosen by mwcc.** `if (x == c) return A; return B;` can come
  out with the condition inverted and the bodies swapped. If the shape is wrong,
  rewrite as `u32 r; if (x != c) r = A; else r = B; return r;`.
- **Conditional stores**: `if (v == 0) b = 0; else b = b + v;` emits `moveq`
  then `addne`; `b = v ? b + v : 0` emits the opposite order.
- **u8 arguments** force `and rX, rX, #0xff` into both branches; declare the
  parameter `u32` and cast `(u8)` at the call site to get a single mask.
- **`| 0x1f | 0x20` is not `| 0x3f`.** Writing one folded constant makes mwcc
  emit a single `orr`; retail sometimes keeps two `orr`s, so split the ORs into
  separate statements (`v = (v & ~0x3f) | 0x1f; v = v | 0x20;`).
- **Small `if` bodies** may be if-converted (`strheq`, `movne`, ...) or branched.
  Inverting the condition (write the large branch first, the small one in the
  `else`) usually reproduces retail's predicated small block.
- **Address materialisation** splits `0x8de8` as `add #0x8d00` + offset `0xe8`
  or as a pooled constant + `add`; the choice follows the exact expression and
  the register allocation, not the arithmetic. When a RMW is four bytes short,
  the address was kept in a register instead of rematerialised — try giving the
  address its own pointer variable, or reading the value into a variable first.
- **Register-allocation ties** (retail keeps the accumulator in `r0` where mwcc
  insists on `r1`) resist declaration order and formulation changes. Record the
  dead end and move on rather than weakening the check.

## Families: one source, many members

Identical-skeleton groups differ only in per-function constants: masks, field
offsets, call arguments, mode bytes, callee names and global addresses. Generate
siblings by substituting **only** those constants. Never substitute pc-relative
pool offsets or branch targets — those are computed by the assembler. Use
comma-free sentinels when scripting the substitutions, and make callee
substitution positional (`names[0]` = first `bl`, `names[-1]` = last), skipping
empty names so a file never ends up with two conflicting `extern`s.

## Bookkeeping and release

- `PROGRESS.md` is generated; never hand-edit it.
- `README.md` carries the headline counts (`1,144 functions`, `66,668 bytes`,
  `matched_functions`, `matched_code`, percentages). Update them when the totals
  change, nowhere else, and keep them identical to `PROGRESS.md`.
- If you work from a separate build tree, the sync direction is
  **build tree → repository for `src/` and `config/`**, and
  **repository → build tree for `README.md`/`PROGRESS.md`**. Use `cp -r`; do not
  use `rsync`.
- Commit in one squashed commit per batch, then `git push --force origin
  HEAD:main`, then upload the objdiff report:
  `python tools/upload_report.py <build>/report.json --commit $(git rev-parse HEAD)`
  with `BYTEMATCH_TOKEN` in the environment.
- The upload is part of every push, not of source changes only: a docs, README
  or LICENSE commit still moves `HEAD`, and a report left behind on an older
  commit makes the published numbers look stale. Push, then upload, in the same
  breath - including when the batch changed no source at all.

## When something goes wrong

- `tools/match/t10.py --check` reports `dangling` (delinks entry with no source),
  `unlinked` (source with no entry) or `dupes`. Fix with
  `python tools/match/t10.py --repair <module>`; it is safe to run at any time.
- A killed process cannot leave a half-linked unit: `try_unit` restores the tree
  in a `finally` block. If you suspect a manual edit broke the tree, run
  `--repair`, then `ninja sha1`.
- If `ninja sha1` fails after your change, `git diff` the affected
  `src/`/`config/` files and revert the offending unit — do not adjust the
  expected digest.

## Non-goals

Do not add features to the game, reformat unrelated files, rename existing
symbols, or "clean up" delinked assembly. Every non-matching byte in the ROM is
a bug; cosmetic changes to matching code can silently change codegen.
