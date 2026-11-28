# Roadmap to a complete decompilation

Forward-looking only. What is already matched is in [`PROGRESS.md`](PROGRESS.md);
how to work is in [`AGENTS.md`](AGENTS.md) and
[`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md).

Every figure below was measured against this tree on **2026-11-26** with
`ninja report`, `tools/match/progress.py`, `tools/match/fresh.py --write`,
`tools/match/famsurvey.py`, `tools/match/sdkmap.py --near 0.85` and a keyword
census of `tools/match/walls-detail.txt`. Re-measure before trusting a number
that decides what to do next; two phases of this file previously pointed at work
that had already been done.

What changed in this revision, and why:

- **The SDK residue is now Phase 1, not absent.** Measured: 23 arm9 units agree on
  85%+ of their bytes with a prebuilt NitroSDK object (7,568 B) and one is exact.
  Three of the last five matches came from this recipe, and it is written down.
- **The ov002 families stay Phase 2 but are no longer sold as one tie away.** The
  drafts compile at 600/608 B and 688-712/756 B today, so the residue is tens of
  bytes of body, and the phase is priced as authoring work.
- **Walls re-measured and triaged**: 598 parked units, 315 notes, and three
  register sub-classes (including two found this week) declared bounded instead
  of queued.
- **Two arithmetic errors corrected**: the finish line's unit count (4,803 was
  stale; the tree has 4,961) and the cost of crossing 10% code (9,930 B, not
  26 KB).

## Where the work is

| Metric | Matched | Total | Left |
|---|---:|---:|---:|
| Functions | 2,007 | 3,806 | **1,799** |
| Code | 193,792 B | 1,841,700 B | **1,647,908 B** |
| Data | 630,724 B | 801,936 B | **171,212 B** |
| Complete units | 4,296 | 4,961 | **665** |

**52.73% of functions but 10.52% of code bytes.** The remaining 1,787 functions
are not a tail of small leaves:

| Size | Functions | Bytes |
|---|---:|---:|
| 0-63 B | 9 | 244 |
| 64-127 B | 68 | 6,928 |
| 128-255 B | 209 | 38,756 |
| 256-511 B | 510 | 183,328 |
| 512-1023 B | 565 | 403,404 |
| 1024-2047 B | 248 | 355,988 |
| 2048 B + | 178 | **659,692** |
| **total** | **1,787** | **1,648,340** |

(The 432-byte difference from the 1,647,908 B in the headline table is delinked
pool words that live inside code ranges but are not functions.)

So 182 functions hold 40% of the remaining code and 987 hold 85%. Small-unit
passes still matter - they are where a lever is proven cheaply - but no amount of
them closes the byte gap.

| Module | Code matched | Bytes left | Functions left | Largest 10 |
|---|---:|---:|---:|---:|
| OV002 | 7.77% of 715,724 B | 660,124 | 861 | 58,752 B |
| OV003 | 3.60% of 586,712 B | 565,564 | 387 | 84,412 B |
| ARM9 main | 38.83% of 224,616 B | 137,388 | 267 | 39,828 B |
| OV005 | 7.63% of 142,964 B | 132,052 | 98 | 46,504 B |
| OV004 | 9.30% of 89,584 B | 81,256 | 90 | 35,580 B |
| OV010 | 5.09% of 25,144 B | 23,864 | 23 | 17,264 B |
| OV009 | 18.20% of 24,788 B | 20,276 | 28 | 13,520 B |
| OV001 | 4.60% of 13,904 B | 13,264 | 16 | 11,148 B |
| OV008 | 31.48% of 10,940 B | 7,496 | 18 | 6,112 B |
| OV006 | 9.56% of 7,324 B | 6,624 | 11 | 6,468 B |

OV002 and OV003 together are 74% of the remaining code. `ov000` and `ov007` are
empty overlays (no `.text`); they have no `src/` directory and never will.

Data has its own shape: 78.65% matched, 82.61% credited as fully linked, and the
31,760-byte difference is one diagnosed cause, not eleven bugs - see Phase 5.

## What actually moves the counter

Ranked by measured yield, not plausibility. Each row is what recent passes
actually produced:

| Lever | Evidence | What it yields, or the rule |
|---|---|---|
| SDK transcription | `OS_SaveContext` (76 B), `FSi_SeekDirCommand` (152 B), `PM_SetBackLightAsync`, `FSi_ReadFileCommand`, the 16 SDK asm units | one unit per short session; the recipe is written down |
| `asm void` sibling of a matched unit | `func_0203b50c` from `func_0203b4e4` (76 B) | near-free when the neighbour is matched |
| `lift.py` (m2c bridge) | decodes a 260 B unit into switch/loops/globals/offsets in one call; used to write the `02080ca4` candidate above | removes the 190-instruction read, not the re-spelling |
| Hand authoring a 60-300 B unit | this project's steady rate | 2-3 units, 150-300 B per pass |
| Family substitution | 31,980 B sitting in ov002's top four groups | the big multiplier, but blocked on decode |
| `permute.py` / `permusweep.py` | returns nothing on register-only residue, matches on the first run at `--min-structural 2` | fire it only on structural residue |
| `grind.py` | journal: 7,329 units, 0 matches, 41,619 variants | spent; do not restart |

Two rules follow. **Refresh the queue before picking** (`fresh.py --write` plus
`git ls-tree HEAD src/<mod>/<name>.c`): one pass this week spent two build cycles
re-deriving `func_02028888` and `func_0202bea8`, both already matched, and moved
no counter. And **the SDK map is nearly drained**: arm9 has 1 exact hit left (an
8-byte stub) and 23 partial hits at 85% or better (7,568 B); ov002, ov003 and
ov005 have *zero* hits at that threshold, so the SDK is an arm9-only source.

## Phase 1 - the SDK residue in arm9

23 unmatched units agree on 85% or more of their bytes with a prebuilt NitroSDK
2.0 RC3 object, and one more is exact. Partial agreement is normal: retail has
relocations applied, the archive object does not. Two recipes are proven:

- **asm block**: transcribe the SDK's own `asm void` body with the offsets
  restated numerically and `ldr rX, =<symbol>` for every pooled callee
  (`src/arm9/func_0202564c.c` is `OS_SaveContext`).
- **C body**: restate the SDK header's structs at the offsets retail uses and
  compile the body unchanged (`FSi_SeekDirCommand`, `PM_SetBackLightAsync`).

The files behind the hits are `os_arena.c`, `os_spinlock.c`, `os_thread.c`,
`spi/ARM9/src/{pm,tp}.c`, `fs/{fs_archive,fs_overlay,fs_command}.c`,
`card_{backup,common,request}.c`, `pxi_fifo.c`, `ctrdg.c`, `wm_system.c`,
`gx_load2d.c`, `gx_util.c`, `rtc_external.c`.

`GXS_LoadOBJ`/`GX_LoadOBJ` (`0x02023df8`, `0x02023e58`, 96 B each) are also the
only "family" `famsurvey` reports for arm9 - they are SDK routines, so they belong
here rather than in Phase 3.

Criterion: each unit is installed by `reg.py` with the digest unchanged. A
partial `sdk.py` hit is provenance, never a match.

## Phase 2 - the OV002 family spine

`famsurvey.py ov002` groups the module's unmatched functions into **53
identical-skeleton groups**; the largest 25 cover 102 functions and 73,024 B. The
top four are one per-entity 3D update at four sizes - **47 functions, 31,980 B**:

| Group | Members | Bytes | Member |
|---|---:|---:|---|
| 756 B | 13 | 9,828 | `0x0208f860` |
| 608 B | 14 | 8,512 | `0x0208f38c` |
| 688 B | 10 | 6,880 | `0x0209ab8c` |
| 676 B | 10 | 6,760 | `0x0208f0e8` |

Both flagship members have their decode on disk and a compiled draft, and both
are parked on body residue, not on search:

| Member | Decode | Best draft (measured today) | What is left |
|---|---|---:|---|
| `0x0208f38c` | `notes/ov002_family_0208f38c.md` - full data map, arithmetic and call table | `build/cand/f_8f38c.c` compiles **600 / 608 B** | 8 bytes of body; the `g`/`rec` colour pair is the known tie |
| `0x0208f860` | `notes/ov002-0208f860.md` - frame, register roles, pool constants | `build/cand/ov2_8f860.c` **688 / 756 B**, `n_8f860_a.c` **712 / 756 B** | 44-68 bytes of body order and spill placement |

This is authoring work, not permutation work: a 756 B function is ~190
instructions and a residue of tens of bytes is a decode disagreement somewhere in
the middle, which `permute.py` cannot invent. Budget one member end to end,
`var3` then `reg.py`, and only then generate siblings.

Criterion: one member of a group passes `reg.py`; then each sibling is produced by
substituting **only** per-function constants (masks, field offsets, callee names,
globals, mode bytes) and each earns its own gate. 10% of the code is 184,170 B,
so 9,930 B stands between the tree and that line - the top two groups (18,340 B)
clear it twice over.

## Phase 3 - the remaining families

| Module | Groups | Functions | Bytes |
|---|---:|---:|---:|
| OV003 | 8 | 17 | 6,228 |
| OV005 | 2 | 6 | 3,272 |
| OV004 | 1 | 2 | 744 |
| OV009 | 1 | 2 | 424 |

27 functions and 10,668 B, same machinery as Phase 2. The ov002 groups beyond the
top four (49 of them) shade into the Phase 6 long tail; treat 512 B and up as
long-tail work and use family substitution only where a member is already matched.

## Phase 4 - wall triage by residue class

`tools/match/walls.txt` parks **598 units**; `walls-detail.txt` holds 315 notes
(one note can cover several units). A keyword census of the notes, classes
overlapping:

| Residue | Notes |
|---|---:|
| register colouring / allocation | 111 |
| folded or short arithmetic | 73 |
| frame slot / spill | 68 |
| branch layout / if-conversion | 52 |
| pool or literal word | 52 |
| BLX interworking | 7 |
| unclassified | 97 |

**Bounded - do not re-spend.** The register class is now measured in three
flavours and none of them has a known source lever: retail keeps a value in `ip`
(the `02068c78` case), keeps a pointer in caller-saved `r1` across a call and
needs no frame (`OS_InitLock`, 240 B), or ties two pointers to `sl`/`r8` while
b56 swaps them. Declaration order, statement order, 24 pragmas and the
SM64DS-style first-demand ordering have all been tried. BLX interworking is the
same story: an ARM caller cannot be made to emit `blx` to a Thumb callee under
any declaration or pragma spelling.

**Live classes**, with the lever that has actually worked:

- **Address macros** (`HI`/`LO` over `&param`): recovered a prologue that looked
  unreachable. Try on every struct-by-reference prologue before writing it off.
- **A constant that must not fold**: retail materialises an *anonymous* zero from
  a pool word - an absolute LCF symbol such as `OSi_SYS_STACKSIZE` - and keeps
  both branches around it (`OS_GetInitArenaHi`). A literal, even a `const`
  initialiser, folds the arm away.
- **Counters and pointer pairs**: two symmetric nested counters and two pointers
  that trade `sl`/`r8` move under a declaration shuffle with initialisers split
  out of the declarations. `permute.py` finds these; `respell.py` does not.
- **Identical arms retail keeps separate**: three copies of a three-arm range
  test with the same body (96 B each, 288 B total) where b56 merges or
  if-converts. Source shape still unknown.
- **imm8 fold / "load issued before the increment"**: find the statement whose
  scheduling produces it; permuting does not help.

Criterion: every wall ends at MATCH or carries a note naming its class, so the
next pass can ignore the bounded ones without re-reading them.

## Phase 5 - data sections

**The relocation gap (31,760 B, 11 units).** These units already score 100.0%
fuzzy but are not credited as matched, which is why published matched data
(630,724 B) sits below fully linked (662,484 B). Their pointer words target
*interior* byte addresses of another module's data blob
(`data_ov005_02080fdf` inside `data_ov005_02080368`; 9,980 B of this is in
`src/arm9/data_0203d908` alone). The names exist in `symbols.txt` as
`kind:data(any)`, but the linker does not export them, so a reference fails with
`Undefined`. Spelling them as container-plus-offset does not work either: mwcc
rejects `(u32)((u8 *)&sym + 0xc77)` in a `const u32[]` initialiser; only a bare
`(u32)&sym` relocates. The fix is a **delinker/config export of those interior
symbols**, then a digest re-verification - 31,760 B of already-correct bytes.

**The real gap (171,212 B)** sits in OV005 (45,808 B), OV006 (33,668 B), OV002
(13,260 B) and OV003 (11,076 B) as unstructured blobs, plus ~1,753 B of missing
carve-outs. Structure recovery, not spelling.

`symreloc.py scan`/`apply` is for faithfulness and moves no published number;
run it after the credited bytes move.

Criterion: `ninja report` data matched rises, digest unchanged.

## Phase 6 - the long tail

178 functions of 2 KB or more are 659,692 B; the largest are:

| Unit | Size |
|---|---:|
| `arm9 0x0200a904` | 14,312 B |
| `ov003 0x0208af10` | 13,540 B |
| `ov005 0x02051cac` | 11,292 B |
| `ov003 0x020589d0` | 11,292 B |
| `ov003 0x020aaa10` | 9,724 B |
| `ov003 0x0207659c` | 9,272 B |
| `ov003 0x0205ee64` | 9,240 B |
| `ov010 0x0204c7c0` | 8,204 B |
| `ov002 0x02055110` | 8,024 B |

This is where the authorised per-module worktrees pay off:
`git worktree add build/worktrees/ov003 -b work/ov003`, one module per worker,
never two workers on one module, workers never push, merge back source plus that
module's `delinks.txt`, then re-gate in the main tree.

Pick by bytes and by shape reuse: an 8 KB function that shares its skeleton with
a matched neighbour is worth more than one that does not. Expect to author a
decode note per large unit - the Phase 2 members show that pays for itself.

Criterion: digest unchanged after every merge.

## Next actions, in order

1. **Phase 1, arm9 SDK residue**: 23 partial hits over 7,568 B, clustered in a
   dozen SDK source files. Start with `os_thread.o` `OS_CreateThread` (264 B) and
   `pxi_fifo.o` `PXI_InitFifo` (260 B), then the `gx_load2d.o` pair (96 B each).
2. **Phase 2, 608 group**: `0x0208f38c` is 600 of 608 B with the decode written.
   One match is worth 14 members and 8,512 B.
3. **Phase 2, 756 group**: `0x0208f860` has two drafts at 688 and 712 of 756 B;
   diff them against each other first - the answer is probably in the difference.
4. **Phase 4, the two live classes with written levers** (address macros, and a
   constant that must not fold) across the parked prologue/arena walls.
5. **Phase 5, delinker export** so 31,760 B of correct data gets credited.
6. **Phase 3, OV003** once one Phase 2 member proves the family flow end to end.

Everything else waits for a lever from one of these six. Wall units outside the
live classes are recorded dead ends, not a backlog.

## Finish line

All four together:

1. `python tools/match/t10.py --check` `ok` for every module - `dangling=0`,
   `unlinked=0`, `dupes=0`.
2. `ninja sha1` prints `9eec3248d3569eba8abcb3244adcbdf01a5ea424` and `OK`.
3. `PROGRESS.md` shows 3,806 / 3,806 functions, 1,841,700 / 1,841,700 code
   bytes, 801,936 / 801,936 data bytes and 4,961 / 4,961 complete units, with
   `README.md` carrying the same numbers and `matched_*` equal to `complete_*`.
4. No `src/` file is a stub, an approximation or a generalised asm block.

## Verification guards

- `progress.py` **exits 1** when any unit scores below 100% in objdiff and names
  it: a unit can be byte-perfect in the ROM and still be counted wrong if its
  source spells a relocated address as a number. Treat the exit status as a
  publish-time gate.
- `objcheck.py --unit <mod> <name>` explains one such unit word by word and
  relocation by relocation. It is a diagnostic, stricter than objdiff, and
  reports address-equivalent findings on a healthy tree.
- Rule of thumb: bytes right but objdiff below 100% means read the relocation,
  not re-spell the code.

## Traps worth restating

- `var3` alone lies: relocations are masked, so a `bl` to a name missing from
  `symbols.txt`, a wrong pooled literal or a numeric address all report `MATCH`.
  `reg.py` and the digest decide.
- A stale `true_fresh.txt` makes matched work look new. Refresh, then check
  `git ls-tree HEAD src/<mod>/<name>.c`.
- A residue is not a licence: never weaken `t10.py`, the expected digest, the
  flags or `configure.py` defaults to close a gap.
- Do not commit the ROM, BIOS or a save; `*.nds`, `arm7_bios.bin` and `extract/`
  are gitignored and untracked.
