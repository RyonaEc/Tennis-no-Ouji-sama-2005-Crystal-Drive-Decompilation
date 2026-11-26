# Roadmap to a complete decompilation

Forward-looking only. For what has already been matched see
[`PROGRESS.md`](PROGRESS.md); for how to work see [`AGENTS.md`](AGENTS.md) and
[`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md).

Every number here was measured against this tree, and every phase has a success
criterion that the digest gate decides, not a percentage alone.

## Where the work actually is

| Metric | Matched | Total | Left |
|---|---:|---:|---:|
| Functions | 1,894 | 3,806 | **1,912** |
| Code | 169,172 B | 1,841,700 B | **1,672,528 B** |
| Data | 630,724 B | 801,936 B | **171,212 B** |
| Complete units | 4,183 | 4,851 | **668** |

The important asymmetry: **49.8% of functions but 9.2% of code bytes.** What
remains is not a long tail of small leaves - it is a few hundred large functions.

Data is the mirror image: 78.65% matched. Its fully-linked figure is higher
(82.61%, 662,484 B) and the 31,760-byte difference is a known, diagnosed gap -
see Phase 4.

By module, sorted by bytes still unmatched:

| Module | Code matched | Bytes left | Functions left |
|---|---:|---:|---:|
| OV002 | 6.49% of 715,724 B | 669,296 | 902 |
| OV003 | 2.51% of 586,712 B | 571,964 | 414 |
| ARM9 main | 36.81% of 224,616 B | 141,932 | 292 |
| OV005 | 6.57% of 142,964 B | 133,576 | 104 |
| OV004 | 7.21% of 89,584 B | 83,124 | 99 |
| OV010 | 5.09% of 25,144 B | 23,864 | 23 |
| OV009 | 15.60% of 24,788 B | 20,920 | 31 |
| OV001 | 4.60% of 13,904 B | 13,264 | 16 |
| OV008 | 27.20% of 10,940 B | 7,964 | 20 |
| OV006 | 9.56% of 7,324 B | 6,624 | 11 |

OV002 and OV003 together are **74% of the remaining code**. `ov000` and `ov007`
are empty overlays (no `.text`, `start == end` in `delinks.txt`); they have no
`src/` directory and never will. That is why they are absent from these tables.

## Phase 1 - the OV002 family spine (highest value)

`famsurvey.py ov002` groups unmatched functions by identical skeleton. The top
four groups alone are **47 functions and 31,980 bytes**, and they are one shape
(per-entity 3D placement) at four sizes:

- `13 x 756 B` = 9,828 B - `0x0208f860, 0x0209a5f4, 0x0209c728, 0x0209eb18, ...`
- `14 x 608 B` = 8,512 B - `0x0208f38c, 0x02096a64, 0x02098aac, 0x020a06e8, ...`
- `10 x 688 B` = 6,880 B - `0x0209ab8c, ...`
- `10 x 676 B` = 6,760 B - `0x0208f0e8, ...`

Two cracked skeletons are worth 27 functions and 18 KB. The project has done
this before: `notes/mwccarm-codegen-solved-families.md` records a 21-function
per-slot state machine and an 8-function flag-setter family.

### Engine status (measured 2026-11-26)

The unattended re-spell engine is spent, and the numbers say so: the grind
journal holds 7,351 visited units with **zero** matches, 870 of them marked
exhausted and 6,458 with no candidate file to re-spell at all. In the same
period hand decode kept landing units. So the bottleneck is not variant search,
it is **authoring C for large functions**, and the plan below is ordered that
way: no work is scheduled against `grind.py` as the primary engine.

Both flagship members have a complete decode on disk and a draft that is short
only on allocation, not algorithm:

| Member | Draft | Retail | Residue |
|---|---|--:|---|
| `0x0208f38c` (14 x 608) | 588 B, 291 slots | 608 B | colouring: retail `g`→r8, `rec`→sl, `sb`→sb, frame 0x3c, `idx` spilled to `[sp+0x20]`; mwcc gives `g`→sl, `rec`→r8, frame 0x34 |
| `0x0208f860` (13 x 756) | 548 B | 756 B | body order after the direction branch (see `notes/ov002-0208f860.md`) |

Tried on `0x0208f38c` and recorded in its note: `off = idx * 0x158` used twice
(forces the shared `mul`, 588 B), `sb` declared before `rec` (flips `sb`, leaves
`g`/`rec` swapped), and `v28` assigned after the `/5` instead of at declaration
(scheduling only). Declaration order moves *which* pointer gets `sb` but not
`g`/`rec` - that pair is the remaining lever, and it is a register-colouring
tie of the kind `notes/mwccarm-codegen.md` documents.

Method, per group: match one member completely (it is a 150-190 instruction
function, so budget real time), then generate siblings by substituting **only**
per-function constants - masks, field offsets, callee names, globals, mode bytes
- exactly as `AGENTS.md` describes, and never pc-relative offsets or branch
targets. Each sibling still goes through `var3` then `reg.py`; a family does not
inherit a match, it earns one per member.

Criterion: every member of a group passes `reg.py` with the digest unchanged.
Success signal: crossing 10% code is +27 KB, which the top two groups alone
cover.

## Phase 2 - the remaining families

Worth doing straight after Phase 1 because the machinery is already warm:

| Module | Groups | Functions | Bytes |
|---|---:|---:|---:|
| OV003 | 9 | 19 | 6,556 |
| OV005 | 2 | 6 | 3,272 |
| OV004 | 1 | 2 | 744 |
| OV009 | 1 | 2 | 424 |
| ARM9 | 1 | 2 | 192 |

Criterion: as Phase 1. Total here is 31 functions and 11,188 bytes.

## Phase 3 - wall triage by residue class

`tools/match/walls.txt` holds **589 parked near-misses**. They are not 589
independent problems; they are a handful of classes. Recent evidence:

- The address-macro class (`HI`/`LO` taking `&param`) turned a wall written off as
  "unreachable prologue, 8-byte struct + 2 unused u32 args" into
  `func_0203684c`'s exact 40 bytes. One lever, one wall, and the same lever
  explained `func_02036874`.
- The imm8-fold class (`func_02023df8`, one instruction short) and the "load
  issued before the increment" class (`func_02037320`, `func_02037788`) each have
  a written residue and no lever yet.

Do this as a batch job, not one wall at a time: read the 427 notes, bucket them by
residue text, then work one bucket to a lever. A bucket that yields a lever turns
into a `respell.py` axis for the whole queue, which is where the automated
sweeper stopped finding anything.

Criterion: every wall ends at MATCH or carries a residue that names its class.

## Phase 4 - data sections

The 171 KB data gap has two parts, and only one of them is C work.

**The relocation gap (31,760 B, 11 units).** `matched_data` is missing for these
units even though each scores 100.0% fuzzy, which is why the published matched
data (630,724 B) sits below fully linked (662,484 B). The cause is diagnosed and
it is not a source bug: their pointer words target *interior* byte addresses in
another module's data blob (`data_ov005_02080fdf` inside `data_ov005_02080368`,
9,980 B in `src/arm9/data_0203d908` alone). Those names are recorded in
`symbols.txt` with `kind:data(any)`, but the linker never exports them, so
referencing one fails the link (`Undefined : "data_ov005_02080fdf"`). Spelling
it as the containing symbol plus an offset does not work either: mwcc rejects
`(u32)((u8 *)&sym + 0xc77)` in a `const u32[]` initialiser with `<string not
found>`; only a bare `(u32)&sym` produces a relocation. Fixing this needs the
delinker to export those interior symbols (config change, then re-verify the
digest), not a spelling hunt. Do that first: it is 31,760 B of already-correct
bytes that only fail to be credited.

**The real data gap.** The rest sits inside OV005 (45,808 B), OV006 (33,668 B),
OV002 (13,260 B) and OV003 (11,076 B) as large blobs that need structure
recovery, plus about 1,753 B of missing delinks carve-outs (mechanical). Treat
the blobs as part of the Phase 5 long tail.

Already done, for the record: `symreloc.py apply` rewrote 1,065 relocated words
across 33 units in arm9/ov002/ov003 from numeric addresses to the symbols
`relocs.txt` records. It changes no published number (objdiff already credited
those words) but it is the faithful spelling, and it fixed the three
code-side relocation findings `objcheck.py` reported.

Criterion: `ninja report` data matched rises, digest unchanged.

## Phase 5 - the long tail

After Phases 1-4 the remainder is large unique functions, overwhelmingly in OV002
and OV003. This is where the authorised per-module worktrees pay off:
`git worktree add build/worktrees/ov002 -b work/ov002`, one module per worker,
never two workers on a module, workers never push, merge back source plus that
module's `delinks.txt` only, then re-gate in the main tree.

Pick by bytes, not count: a 1,028-byte function is 257 instructions of progress
in one unit. Work the biggest unmatched functions in the module the worker owns.

Criterion: digest unchanged after every merge.

## Finish line

Done means all four of these, together:

1. `python tools/match/t10.py --check` `ok` for every module, `dangling=0`,
   `unlinked=0`, `dupes=0`.
2. `ninja sha1` prints `9eec3248d3569eba8abcb3244adcbdf01a5ea424` and `OK`.
3. Functions 3,806 / 3,806, code 1,841,700 / 1,841,700 B, data 801,936 /
   801,936 B, complete units 4,803 / 4,803 in `PROGRESS.md`, with `README.md`
   showing the same numbers - and `matched_*` equal to `complete_*` throughout,
   which `progress.py` now enforces (see Verification guards).
4. No `src/` file is a stub, an approximation or a generalised asm block.

## Verification guards

Two checks exist because a unit can be byte-perfect in the ROM and still be
counted wrong:

- `python tools/match/progress.py` **exits 1** when any unit scores below 100%
  in objdiff, naming the unit and its matched/complete bytes. This is the
  publish-time gate: ByteMatch reads `matched_*` as it stands, so one 99.9% unit
  makes the site publish "matched 8.87%" against "fully linked 8.88%". That is
  what `src/arm9/func_02004a14` did - 232 bytes and one function - because
  `ldr r0, =0x027c0000` was spelled as a number instead of `=data_027c0000`, so
  the object carried no relocation where retail has one. `ninja sha1` and
  `var3` both mask relocated words and passed it.
- `python tools/match/objcheck.py --unit <mod> <name>` explains *why*: it
  compares the unit's object with the delinked retail object word by word and
  relocation by relocation. It is a diagnostic, not a gate - it is stricter
  than objdiff and reports address-equivalent findings (a `data_x[n]` access
  that retail records against the next symbol, `bl` placeholders a delink
  leaves as `ebfffffe`) on a healthy tree.

Rule of thumb: if a unit's ROM bytes match but objdiff does not give it 100%,
stop and read the relocation, do not re-spell the code blindly.

## Traps worth restating

- `var3` alone lies: a C `*` on doubles emits `bl _dmul`, which is not in
  `symbols.txt`, and the relocation bytes are masked - it reported `MATCH` while
  the linked ROM differed. `reg.py` and the digest decide.
- A residue is not a licence. Never weaken `t10.py`, the expected digest, the
  flags or `configure.py` defaults to close a gap.
- Do not commit the ROM, BIOS or a save. Verified in this tree: `*.nds`,
  `arm7_bios.bin` and `extract/` are gitignored and untracked.
