# Roadmap to a complete decompilation

Forward-looking only. For what has already been matched see
[`PROGRESS.md`](PROGRESS.md); for how to work see [`AGENTS.md`](AGENTS.md) and
[`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md).

Every number here was measured against this tree, and every phase has a success
criterion that the digest gate decides, not a percentage alone.

## Where the work actually is

| Metric | Matched | Total | Left |
|---|---:|---:|---:|
| Functions | 1,903 | 3,806 | **1,903** |
| Code | 171,308 B | 1,841,700 B | **1,670,392 B** |
| Data | 630,724 B | 801,936 B | **171,212 B** |
| Complete units | 4,192 | 4,862 | **670** |

The important asymmetry: **50.00% of functions but 9.30% of code bytes.** What
remains is not a long tail of small leaves - it is a few hundred large functions.

Data is the mirror image: 78.65% matched. Its fully-linked figure is higher
(82.61%, 662,484 B) and the 31,760-byte difference is a known, diagnosed gap -
see Phase 4.

By module, sorted by bytes still unmatched:

| Module | Code matched | Bytes left | Functions left |
|---|---:|---:|---:|
| OV002 | 6.66% of 715,724 B | 668,024 | 897 |
| OV003 | 2.54% of 586,712 B | 571,800 | 413 |
| ARM9 main | 36.84% of 224,616 B | 141,876 | 291 |
| OV005 | 7.02% of 142,964 B | 132,932 | 102 |
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

`permute.py` plus `permusweep.py` is the live search engine; `grind.py` is spent
and should not be restarted. The engine's limit is measured, not assumed: fed the
queue by proximity it keeps picking register-only walls and returns nothing,
while restricted to candidates with a real structural difference
(`--min-structural 2`) it matches on the first run. Register residue is different
work rather than harder search, and the sweep now defaults away from it.

So the bottleneck for the families below is **authoring C**, not variant search:
a 756 B function with 60 structural slots of residue is a decode problem, not a
permutation problem.

Both flagship members have a complete decode on disk and a draft that is short
only on allocation, not algorithm:

| Member | Draft | Target | What is left to author |
|---|---|--:|---|
| `0x0208f38c` (14 x 608) | 588 B | 608 B | `g`/`rec` colouring: retail `g`→r8, `rec`→sl, frame 0x3c, `idx` spilled to `[sp+0x20]`. The shared `mul` for `off = idx * 0x158` is already exercised. |
| `0x0208f860` (13 x 756) | 732 B | 756 B | 24 B of body order after the direction branch; decode in `notes/ov002-0208f860.md`. |

For the 608 group the lever to try is the `g`/`rec` pair itself: declaration
order moves `sb` but not that pair, so it is the documented colouring tie until
some spelling moves it - and a moved pair is worth 14 members.

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

`tools/match/walls.txt` holds **468 parked near-misses** (count them with
`grep -vc '^#' tools/match/walls.txt`). They are not 468 independent problems;
they are a handful of classes, and one of them is measured as unreachable and
should be triaged out rather than worked:

**The register-permutation class.** Every wall whose only residue is register
choice - six ov002 prologue walls (`020b5d28`, `020c0c70`, `020c82d4`,
`020f4490`, `020da96c`, `020f466c`), `02068c78`, `020604b4`, and arm9's
`0202c430` - has now been attacked with declaration order, statement order and
moves, init split/join, additive-chain order, signedness swaps, 24 pragmas, and
the SM64DS-style first-demand ordering. None of it moves the allocation. The
canonical case is `02068c78` (64 B): the candidate has every instruction right
and four slots rotated, because retail holds the base in `ip` - a compiler
temporary - and b56 picks `r3`. No source spelling reaches `ip`. Treat these as
bounded: record the residue, do not re-spend search time.

The classes that are still worth working, with the lever to try on each:

- **Address macros** (`HI`/`LO` taking `&param`): this lever already reproduces a
  prologue that looked unreachable, so try it on the parked prologue walls that
  take a struct by reference before writing them off.
- **imm8 fold** (one instruction short) and **"load issued before the
  increment"**: residues are written and no lever is known; the route is to find
  the source statement whose scheduling produces the early load, not to permute.
- **One-instruction-short folded arithmetic**: where retail keeps a two-step
  computation (`(x - c) - c`) that b56 folds into one, no spelling has defeated
  the folder yet - treat as bounded unless a non-foldable intermediate exists.
- **Counters and pointer pairs**: two symmetric nested counters, and two
  pointers that trade `sl`/`r8`, both move under a declaration shuffle with the
  initialisers split out of the declarations. `permute.py` finds these; `respell`
  does not.
- **Identical arms that retail keeps separate**: three copies of a three-arm
  range test with the same body (`ov003 0x02082220`, `ov003 0x020c806c`,
  `ov005 0x0205ac30`, 96 B each) and the opening chain of `ov005 0x0204e99c`.
  b56 merges or if-converts the arms in every spelling tried, so the source
  shape that keeps them apart is still unknown; 288 B sits behind one lever.

Do this as a batch job, not one wall at a time: read the notes, bucket them by
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

`symreloc.py scan`/`apply` is the tool for the numeric-address sweep that belongs
with this phase. It moves no published number, so run it for faithfulness once
the credited bytes above have moved, not before.

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

## Next actions (in order)

1. **Phase 1, 608 group**: `0x0208f38c` is 588 B of 608 B and its only residue is
   the `g`/`rec` colouring pair. One match is worth 14 members and 8.5 KB.
2. **Phase 1, 756 group**: `0x0208f860` needs 24 B of body order; the decode is
   already written, so this is a re-order and re-gate, not a re-read.
3. **Phase 3, address-macro bucket**: apply the lever that recovered the
   prologue wall to the other struct-by-reference prologues in the wall index.
4. **Phase 2, OV003**: nine groups, 19 functions, 6,556 B, same machinery as
   Phase 1 once one member is authored.
5. **Phase 4, delinker**: export the interior data symbols so the 31,760 B of
   already-correct pointer words get credited; it is a config change plus a
   digest re-verification, not a spelling hunt.

Everything else in the wall index waits for a lever from one of these five; the
queue (large unique functions in OV002/OV003) is Phase 5 and needs a worktree per
module.

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
  in objdiff, naming the unit and its matched/complete bytes. ByteMatch publishes
  `matched_*` as it stands, so a unit that is byte-perfect in the ROM but spells
  a relocated address as a number keeps `matched_*` below `complete_*`. Treat the
  exit status as a publish-time gate and use `objcheck.py --unit` to read why.
- `python tools/match/objcheck.py --unit <mod> <name>` explains *why*: it
  compares the unit's object with the delinked retail object word by word and
  relocation by relocation. It is a diagnostic, not a gate - it is stricter
  than objdiff and reports address-equivalent findings (a `data_x[n]` access
  that retail records against the next symbol, `bl` placeholders a delink
  leaves as `ebfffffe`) on a healthy tree.

Rule of thumb: if a unit's ROM bytes match but objdiff does not give it 100%,
stop and read the relocation, do not re-spell the code blindly.

## Traps worth restating

- `var3` alone lies: relocations are masked, so a `bl` to a helper that is not
  in `symbols.txt`, a wrong pooled literal, or a numeric address in place of a
  symbol all still report `MATCH`. `reg.py` and the digest decide, and
  `objcheck.py` scores the relocations.
- A residue is not a licence. Never weaken `t10.py`, the expected digest, the
  flags or `configure.py` defaults to close a gap.
- Do not commit the ROM, BIOS or a save. Verified in this tree: `*.nds`,
  `arm7_bios.bin` and `extract/` are gitignored and untracked.
