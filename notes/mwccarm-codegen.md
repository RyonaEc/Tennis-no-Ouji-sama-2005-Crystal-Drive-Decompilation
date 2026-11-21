# mwccarm codegen levers (ATOJ)

Source-shape levers verified against this cartridge. Nothing here predicts the
allocator: a lever is an axis to move, and every candidate still has to be
compiled and byte-diffed before it counts. Evidence for each row - measured
diffs, bodies tried, verdicts - is in
[`mwccarm-codegen-log.md`](mwccarm-codegen-log.md); section numbers in
`build/walls.txt` and commit messages point there.

Toolchain: CodeWarrior `mwccarm`, `-O4,p`, ARM mode, struct alignment 4.
Retail is a **split build**: `1.2/sp2` for most units and `1.2/b56` for the ones
listed in `config/ATOJ/mwcc_versions.txt`. Never add a version or a map entry
without a ROM-digest match.

## How to work

* The digest is the only proof: `ninja sha1` must print the retail hash. An
  object diff, `t10.try_unit` alone, or a `MATCH` with relocations masked does
  not close a unit.
* Probe compilers before rewriting. `python build/vprobe.py <mod> <addr> <size>
  <file.c>` runs `1.2/b56` then `1.2/sp2` and masks relocations. In the small
  bands every recent hit was b56-only.
* Work from the live queue, not a stored list: a unit is unmatched only if
  `src/<mod>/<name>.c` is absent. Symbol names come from `symbols.txt`
  (`0x02085ff8` is `func_ov002_0202085ff8`), never from the address alone.
* Read register *roles*, not numbers. Fix the **first** structural divergence,
  then colours; do not chase the diff count.
* Reference globals by symbol. An absolute address can compile to identical
  bytes and then fail to link.
* Park after about two spellings. Record the body and the exact residue in
  `build/walls.txt`, then move on; a wall is a result, not a failure.
* Solved families do not need re-deriving: if `src/` already has the unit, its
  bytes are settled. The log's §9-§13 are history, not a to-do list.

## Symptom → lever

| What the diff shows | Try first | Log |
|---|---|---|
| Whole function consistently renamed | declaration order (callee-saved locals from the last declaration backwards), then first-use order | 3.1 |
| Retail `add rX, base, #off` then `[rX]`; you fold into `[base, #off]` | pointer local used by both the load and the store | 2.1, 2.1a |
| You emit an `add` retail does not have | collapse the address into the load/store expression | 2.1 |
| Retail loads the module base twice, you load it once | write the global expression twice instead of caching it | 2.2, 2.58 |
| Retail stores a field and immediately reloads it | write the field twice; do not hold it in a local | 2.3, 2.57 |
| Offset split as `#0x3000` + small immediate | the offset is a value (`u32 off` local), not a literal | 2.4 |
| Retail pools a constant you split (or the reverse) | keep the value in a local when it survives across statements; a one-expression constant is a literal | 2.10, 2.14, 2.23 |
| RMW lands four bytes short | compound assignment on a freshly computed lvalue (`*(u16 *)(p + 0xc) -= 1;`) | 2.63 |
| Retail stores a constant to the stack and reloads it (`str r1,[sp]; ldr r0,[sp]`) | the value is a `volatile` local; a plain literal emits none of the spill | 2.65 |
| Retail re-materialises the address at the write | re-derive it at the store instead of keeping the base live | 2.60 |
| You get `movhs`/`movlo`, retail `movge`/`movlt` | the compared operands must be signed (`s32`) | 3.3, 2.55 |
| `and rX, rN, #0xff` hoisted to the caller | declare the callee parameter narrow and let the call convert | 3.2 |
| Callee result is live while other setup happens | call it after the setup, or use the result immediately | 10.1 |
| Retail keeps a global in a register across calls | do not take its address; re-reference the symbol | 2.58 |
| Frame/`stmdb` with an argument block | by-value struct parameter or four live arguments, not more locals | 2.7, 2.54 |
| Retail branches, you get `movcc`/`strcc` (or reverse) | `return a \|\| b;` branches; an early-return ladder predicates | 4.1 |
| Several conditions all jump past one call | it is one short-circuit `&&` around the call, not early returns | 2.40 |
| Small `if` body predicated the wrong way | write the large branch first and the small one in the `else` | 4.1 |
| Block order does not match | `goto` labels force layout and defeat if-conversion | 2.25, 2.30 |
| Case blocks out of order / dense switch becomes a chain | labels in block order; a returning ladder lowers to a range check | 4.3, 2.9 |
| Jump table shape is wrong | hand-decode it; case order comes from the source labels | 2.11-2.13 |
| Loop counter and pointer bump in the wrong order | increment the counter first; give a large step its own statement | 4.2 |
| Loop bound reloaded every iteration | hoist the bound into a variable | 10.3 |
| Signed division by 2 missing `add lsr #31` | write `/ 2` on an `s32`, not `>> 1` | 4.4 |
| `strh`/`strb` post-index missing | step wider than the store width, written as its own statement | 4.2 |
| Struct field access folds where retail adds | use a struct-typed access, not `u8 *` arithmetic | 2.55, 2.63 |
| Two adjacent words copied separately | struct assignment (`*(P2 *)dst = *(P2 *)src;`) batches loads then stores | 2.5 |
| Bitfield read uses `asr` | declare the member `s32 b : 1`; `u32` emits `lsr` | 2.59 |
| Tail call appears as `ldr ip, =callee; ...; bx ip` | write the call in tail position | 11.2 |
| `mov r0, #0; bx lr` | `return 0;` | 11.1 |
| Same body resists both compilers in the 68-88 B band | check the b56 address clusters before rewriting again | 2.46, 2.52, 2.63 |
| Candidate byte-matches but the link fails | absolute address where a symbol belongs, or a name/address mismatch | 2.64 |
| A failed compile looks like a match | treat an empty object as no match and recompile | 2.35 |
| Object diff says MATCH, ROM digest fails | SWI stubs and constant pools; only `ninja sha1` decides | 2.45 |

## Classes that are not reachable from C (park them)

* **Register-role ties**: the instruction stream is identical but two registers
  are swapped and every spelling keeps the swap.
* **Address materialisation**: retail keeps or remakes `base + offset` where
  mwcc folds it, or the reverse.
* **`ldm`/`stm` block copies**: retail's frameless `ldm r0!`/`stm r1!` pairs are
  not produced by any C shape tried; struct assignment adds a frame.
* **Odd frame padding**: a phantom 4-byte slot with no call and no stack access.
* **Predicated byte-copy loops** (`stmlt`/`ldmlt` with rotation).
* **BIOS SWI stubs**: 4-6 byte wrappers whose object compare can pass while the
  linked ROM does not.

Each has a body and a diff summary in `build/walls.txt`.

## Where to look

* `notes/mwccarm-codegen-log.md` - dated evidence, superseded rules, per-unit
  bodies and the walls as they were found.
* `build/walls.txt` - every parked unit, one line each, with the exact residue.
* `AGENTS.md`, `CONTRIBUTING.md` - the hard rules, the edit loop and the
  release procedure.
