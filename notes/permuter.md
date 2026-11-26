# The permuter: what it searches, how it scores, and why

`tools/match/permute.py` searches source shapes for one function against retail
bytes. It exists because the remaining walls are register-allocation residue:
the instruction stream is already right and no spelling change is obvious from
the disassembly.

## What other projects do, and what is worth copying

`simonlindholm/decomp-permuter` randomises the C file and scores an objdump
diff. Its own FAQ says the score "should probably be tweaked to look at e.g. the
register diff graph", and it documents the metric the ecosystem settled on,
shared by asm-differ and objdiff:

| Difference | Penalty |
|---|---|
| register choice | 5 |
| stack offset | 1 |
| immediate | 1 |
| reordering | 60 |
| insertion or deletion | 100 |

Two projects had to replace that scorer, and the reasons matter here:

* **BFM-decomp** found relocation *names* being scored as diffs, so every
  relocation slot the search could never change was a permanent penalty and the
  score floated on a floor with no gradient to zero.
* **rac3-decomp** replaced the stock scorer with an aligned diff (score = 10 x
  aligned diffs) because the stock weights disagreed with how close a function
  really was.
* **rac1-decomp** and **k1's boot_elf run** both report that a large share of
  "improvements" from random mode change what the code does - one survey puts it
  at roughly half - so every output needs re-checking.

## What this project's permuter does

* Aligns instructions with `difflib` on a key that keeps mnemonic and operand
  shape but blanks registers, immediates and stack offsets, so one insertion
  cannot cascade into a wall of unrelated diffs.
* Charges the penalties above, and orders candidates by
  `(structural pairs, weighted cost, size delta)`. Structural first matters:
  a regperm one instruction from done should always beat a structurally wrong
  candidate with a cheaper register bill.
* Masks what the linker fills in before the byte compare (`fastmatch.masked`),
  so a call stub in a bare compile is never scored as a hit - the BFM-decomp
  lesson.
* Only offers rewrites that preserve meaning: statements reorder only when the
  read/write sets are disjoint and neither side is a call or control flow;
  `if/else` becomes a conditional expression only when both arms assign the same
  lvalue; `u32`/`s32` toggles only touch simple counters. Upstream accepts
  semantic damage and relies on a human to catch it; this tool refuses it, which
  is also why its winners are usable source rather than hints.
* Exhaustive declaration order first (`--sweep-max`, default 6 locals; 720 orders
  is a few seconds at ~25 ms per compile), then a seeded annealed search with
  restarts, a mutation bandit and optional worker processes.

`--temps` is off by default. It splits a pure arithmetic expression through a
local, which changes live ranges on purpose; such a result is printed
NEEDS-EXPLANATION and must be justified before anything is kept.

## Measured results (2026-11-26)

* Self-test: a matched 144 B unit with its seven declarations shuffled (beyond
  any rotation) is recovered byte-exactly by the declaration sweep.
* `func_ov002_0209d988` (328 B) had been parked at 37 differing slots, then 17
  with the earlier version. With the aligned weighted score the search reached
  an exact match in 192 candidates / 6 s, from `join init y + decl 0/1`: the
  winning shape declares `u32 z;` without an initialiser, keeps `s32 y = 0;`
  initialised at its declaration, and writes the sign choice as `if`/`else`
  rather than a conditional expression.
* `func_ov002_020c5428` (328 B) is the same body with `0x39` -> `0xb8` and
  matched with the same source.

## Still missing

* No frame/regperm/structural triage of the unmatched queue: decomp-triage's
  split (frame-only failures saturate objdiff and give the permuter nothing to
  optimise toward) would say which walls are worth handing to this tool.
* No batch driver: the 34 angle-family members and the other callers of
  `func_ov002_02057068` still need a per-candidate run, or a manifest-driven
  sweep, before this scales past one wall at a time.

## Batch triage (2026-11-26, `permusweep.py`)

`permusweep.py` scores every candidate in `build/cand` once and sorts by
(structural pairs, weighted cost), which is the triage the section above asked
for. First run over ov002, 66 candidates:

* 8 candidates were pure register renaming (structural 0) or one instruction off
  (structural 1) - exactly the population the permuter can address.
* `func_ov002_0208a3cc` (156 B, a four-slot VMU loop) matched and installed: the
  declaration sweep alone solved it, no random search needed.
* Every other wall in the top eight is the same class, and it is worth naming:
  **the `mul`/`ldr` prologue exchange**. Retail does `mov r1,#0x158; mul r2,r0,r1`
  with the module base in r1; b56 does `mul r1,r0,r1` with the base in r2, or the
  reverse on other units (`0x020c0c70`, `0x020c82d4`, `0x020da96c`, `0x020f4490`,
  `0x020f466c`, `0x020b5d28` all share it). Declaration order, `u32 off`,
  declare-then-assign and inline-at-both-uses have all been tried by hand; the
  permuter's axes do not reach it either, so this is a compiler-behaviour
  question, not a search-space one.

### A zero score is not a match

`func_ov002_020c2b18` reached structural 0 / cost 0 and was still **rejected by
the ROM digest**. That is the documented hazard in this repository - a masked
object compare agrees while the linked ROM differs through constant pools and
pc-relative displacements. `permute.py` now returns 3 (not 0) when `--install`
is rejected, and `permusweep.py` prints those as "scored 0 but the ROM digest
rejected it" rather than counting them as matches.

## New axis: additive-chain order (2026-11-26)

`mut_addchain` reorders the operands of a side-effect-free additive chain
(`g + a0 * 0x158 + 0x2e6`), which is a source-level change that moves *which*
register holds the stride product. It was added because six ov002 walls turned
out to be one class: `0x020c0c70`, `0x020c82d4`, `0x020da96c`, `0x020f4490`,
`0x020f466c`, `0x020b5d28` all keep the module base and the `a0 * 0x158` product
in the opposite registers to retail (`mul r3,r5,r0` with the base in r2 versus
`mul r2,r5,r0` with the base in r3, or the mirror).

Result: every one of the six moved to exactly **1 structural difference**,
weighted cost 91-97, at retail's exact size. So the axis does help - they were
further away before - but none crossed, and the residue is now a single paired
divergence in the prologue. Declaration order, init split/join, statement order,
`u32 off`, declare-then-assign and chain order have all been searched
exhaustively or near-exhaustively on these; the permuter is not the tool that
will close them. The next lever worth trying is a per-unit pragma via
`tools/match/pragma_probe.py`, not more source shapes.

## Scorer fixes: linker targets, padding, and a false "match" (2026-11-26)

`--explain` (new) prints the aligned difference with its penalty kind, and using
it on the ov002 prologue walls exposed two artifacts in the score:

* **Call targets.** A bare compile leaves a stub offset in `bl #...`; retail has
  the linked address. Those were being charged as immediate differences - the
  exact trap BFM-decomp patched out of decomp-permuter. `bl`/`blx`/`b` with an
  immediate target now align on the mnemonic and are not charged for it.
* **Trailing padding.** Zero words at the end of a function decode as `andeq` /
  `andseq` and were counted as a replaced instruction (60). Trailing padding is
  now trimmed from both sides before alignment.

With those two fixed, the six prologue walls read **structural 0** - pure
register residue - where they previously looked structural. The residue is
11-19 register differences (weighted cost 55-95) and a 300 s / 6-island run on
the closest one did not reduce it.

Also fixed: the search treated `structural == 0` as a match, so a register-only
difference could be written out as a "match". A match now requires both the
structural count and the weighted cost to be zero, and `--install` still has the
final say through `t10.try_unit`.

## Research: why the stride-product register will not move

A web-research pass over CodeWarrior mwccarm projects found one close published
analogue - SM64DS's mwccarm notes describe a 2004/b56 function where the ROM has
the table base in `r2` and the stride product in `r1` while every spelling that
computes the stride first produces the opposite, treated there as a frame-layout
floor rather than a source-shape problem
(<https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/mwccarm-codegen.md>).
That note's levers, in order of promise, were: change the *first-demand order of
real values* around the multiply (moving genuine loads/captures earlier), type a
temporary to the element's real type (`short` rather than `int`), and use a
truthful typed array subscript. It found no case where `&arr[a0]` alone flips this
base-versus-product assignment.

The repository already tries the first two by hand and the permuter now searches
statement moves (`mut_move_stmt`) and additive-chain order (`mut_addchain`) as
well. None of it closes the six, which is consistent with the SM64DS reading that
this particular register role is fixed by the compiler's allocator once the
function's live-range shape is what it is.

Next lever, not yet tried: represent the entity table as a real array of a record
type and index it (`&g->ent[a0]` with an element size of 0x158) so the stride comes
from the type rather than from an explicit multiply. That changes the expression
mwcc sees without changing the meaning.

## Self-test, and two bugs it would have caught (2026-11-26)

Three defects in two turns is enough to justify a check on the check. `permute.py
--self-test [module]` takes matched units and asserts two properties:

1. a matched unit's own source scores a true `(0, 0, 0)`;
2. reversing its declarations makes it score non-zero, and the declaration sweep
   recovers the exact match.

It also exercises the module load and the alignment path, which is where the
other two bugs lived. Current state: ov002 and arm9, 8 units each, 0 failures.

Bugs this class of test would have caught immediately:

* the search treating `structural == 0` as a match (a register-only difference
  reported as done);
* `gi`/`wi` referenced after the linker-target guard skipped their assignment,
  which crashed on any instruction whose operands are only registers - it only
  showed up on arm9 candidates, not ov002.

## Triage beyond ov002

`permusweep.py --dry-run` over the other modules: arm9 has 46 candidates, ov003
17, ov004 6, ov005 3. The best arm9 one is `func_0202c430` (24 B): structural 0,
weighted cost 20 - four register differences - and its wall note already records
that var3 matches while the ROM digest rejects, with retail keeping the global
address in `ip` where mwcc uses `r1` for every spelling tried. That is the same
class as the ov002 prologue walls: regperm, unreachable from the source shapes
we can generate.
