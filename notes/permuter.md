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
