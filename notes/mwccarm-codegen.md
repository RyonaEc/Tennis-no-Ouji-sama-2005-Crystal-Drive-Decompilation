# mwccarm codegen levers (ATOJ)

Source-shape levers verified against this cartridge. Nothing here predicts the
allocator: a lever is an axis to move, and every candidate still has to be
compiled and byte-diffed before it counts. Evidence for each row - measured
diffs, bodies tried, verdicts - is in
[`mwccarm-codegen-log.md`](mwccarm-codegen-log.md); section numbers in
`tools/match/walls.txt` and commit messages point there.

Toolchain: CodeWarrior `mwccarm` 1.2/**b56** - build 56, `Runtime Built: Sep 16
2004`, NITRO V1.0 - with `-O4,p`, ARM mode, struct alignment 4. That single
build compiles the whole ROM; `config/ATOJ/mwcc_versions.txt` is empty.

**b56 is retail's compiler, and the digest is how that was settled.** The game
shipped 2004-12-30, so `1.2/sp2` (build 79, `Runtime Built: Aug 11 2005`) cannot
be the launch compiler at all, while b56 does fit the date. The decisive test is
not the date argument though: `configure.py ATOJ --mwcc-version 1.2/b56` plus
`ninja sha1` reproduces `9eec3248...` with **all 1,536 units and no per-file
overrides**. The old sp2 default needed 72 overrides to reach the same digest,
so it was measuring the wrong baseline all along.

Two units made b56 look impossible, and both were our own source shape:

* `func_02025c30` - the cache-invalidate loop. Written statement-level, b56
  folds `add` + `cmp` into `adds`; written function-level (`asm void f(void)`)
  both b56 and sp2 emit it verbatim. See the inline-assembly rule below.
* `func_ov004_0205e274` - `*q = 0; f(p + 0x184, *q);` lets sp2 reload and b56
  forward the stored zero. Writing the argument as `*(u32 *)(p + 0x180)` rather
  than through the cached `s32 *q` matches b56 without `volatile`.

Practical rule: probe b56 first, and treat an sp2-only match as a source shape
not yet found. `sp2` stays worth a second probe (`vprobe.py` sweeps every
vendored build) - this cartridge has 59 units where 1.2 builds disagree, so a
unit that only matches elsewhere is a hint that the shape, not the compiler, is
wrong. Never switch compilers unless the digest still matches afterwards.

### Inline assembly: statement level is optimised, function level is not

`asm { ... }` **inside** a function body is ordinary IR to mwcc: b56's optimiser
rewrites it, so `add` + `cmp` + `bne` collapses to `adds` + `bne` and a block
derived straight from retail comes out 4 bytes short. `asm void f(void) { ... }`
(function-level assembly) is assembled and emitted as written by b56 and sp2
alike. Delinked assembly units must therefore be ported to the function-level
form with an explicit `bx lr`, and `func_02025c30` shows the difference:
statement-level gives 40 B of b56 output, function-level gives retail's 44 B
byte for byte. Do not read a statement-level `asm` mismatch as a compiler
fingerprint - it is a property of the source.

## Compiler identity in this tree

Every directory under `tools/mwccarm/` is a different executable, and the
directory name is an archive label, not a version. Ask the executable:
`mwccarm.exe -V` prints `Version 2.0 build NN (build 00NN)` and a build date, and
the object it produces carries `Metrowerks C/C++ for ARM <version>` in
`.comment`. Measured here (2026-11-22):

| tag | banner | build date | `.comment` |
|---|---|---|---|
| `1.2/b56` | 2.0 build 56 | Sep 16 2004 | `2.0b0055` |
| `1.2/base` | 2.0 build 72 | May 13 2005 | `2.0.0.72` |
| `1.2/sp2` (default) | 2.0 build 79 | Aug 11 2005 | `2.0.0.79` |
| `1.2/sp2p3` | 2.0 build 82 | Oct 11 2005 | `2.0.0.82` |
| `1.2/sp3` | 2.0 build 84 | Oct 28 2005 | `2.0.0.84` |
| `1.2/sp4` | 2.0 build 87 | Apr 7 2006 | `2.0.0.87` |
| `2.0/base` | 3.0 build 114 | Mar 30 2006 | `3.0.0.114` |
| `2.0/sp1p2` | 3.0 build 126 | Oct 13 2006 | `3.0.0.126` |

`b56` is the 2004 compiler (the one sm64ds pins as `2004/b56`), not a 1.2
service pack: Nintendo's build rules map NITRO V1.0 to compiler build 56 and
linker build 67. The same file records separate `SDK_CW_BUILD_NUMBER_CC` and
`SDK_CW_BUILD_NUMBER_LD` values, so a build number read out of a binary may be
the **linker's** - NITRO V0.5 is CC 46 / LD 58, V0.6 is CC 50 / LD 62, DS V1.2 is
CC 73 / LD 72, V1.2 SP2+patch CC 80 / LD 74, V1.2 SP3 CC 84 / LD 77, V1.2 SP4
CC 87. Only the compiler banner identifies a compiler. Note also that the 2004
build's `.comment` says `2.0b0055` while its banner says build 56, so `.comment`
alone is one build off for that one - the banner wins.

### What the `1.2/b56` directory actually contains

It is a composite, and only the compiler decides bytes:

* `mwccarm.exe` is build 56, `Runtime Built: Sep 16 2004 13:20:23`, copyright
  2004, SHA-1 `876570fc9bfada25e93192d414cfb6781ba9dfc3`. SM64DS records
  `8eb0b965…` for its build-56 copy, so ours is a different packaging of the same
  build - the banner and the digest decide, not someone else's file hash. Build
  56 came out of a public Metrowerks `CW_ARM_2.1.1_Update.exe`; the circulating
  24-build `mwccarm.zip` does not contain it, because it floors at build 72.
* `mwldarm.exe` in the same directory is **build 72, `Runtime Built: May 12
  2005`** - a year newer than the compiler. Nintendo's NITRO V1.0 pair is CC 56 /
  LD 67, and build 67 is in no public archive, so the folder was assembled with
  the nearest linker available. `tools/mwccarm/1.2/sp2/mwldarm.exe` is build 74.

The mix is normal for a 2004 build recovered from a compiler-only update, and it
is harmless here: `configure.py` passes explicit addresses and section order
from dsd's delinks/relocs, and the full digest still matches, so the 2005 linker
reproduces retail's packing byte for byte. It would only matter if a failure
ever looked link-level (section padding, section order, padding between
functions) instead of code-level - that is when chasing build 67 would be worth
it.

For reference, Nintendo's release-to-build mapping (from `modulerules.cctype`,
as recorded by the SM64DS archive search) is CC 0038 for IRIS V0.2 and NITRO
V0.3, 0043 V0.4.1, 0046 V0.5, 0049 V0.5+HF1, 0050 V0.6/V0.6.1/V0.7, **0056
NITRO V1.0**, 0068 NINTENDO DS V1.0.2, 0073 V1.2, 0074 V1.2 SP1, 0080 V1.2
SP2+patch, 0084 SP3, 0087 SP4. Only build 56 is reachable publicly; everything
before it is unrecovered, and build 68 is absent from the circulating packs too.

## File layout inside an object

mwcc emits **one `.text` section per function, last function first**: a file
declaring `alpha` then `beta` produces a `.text` for `beta` and then one for
`alpha` (verified against `1.2/sp2` on 2026-11-22). A single-function unit is
unaffected, but two functions in one file exist in reverse source order, and
`fastmatch.text_of` returns only the first `.text`, which is why the harness and
`src/` are one function per file. Definition order also decides what mwcc is
allowed to inline: a same-file callee defined *before* its caller can be inlined,
so a `bl` in retail to a function you keep seeing inlined means that callee
belongs **after** the caller. acww records the same two rules, plus a size-sorted
`data`/`bss` order, in `docs/matching.md`.

Our flags already carry `-sym on`, so every candidate object has DWARF;
`python tools/match/slots.py <file.c>` reads it back and prints the register or
stack slot each parameter and local got - use it instead of guessing on a
register-role wall.

## How to work

* The digest is the only proof: `ninja sha1` must print the retail hash. An
  object diff, `t10.try_unit` alone, or a `MATCH` with relocations masked does
  not close a unit.
* Probe compilers before rewriting. `python tools/match/vprobe.py <mod> <addr> <size>
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
  `tools/match/walls.txt`, then move on; a wall is a result, not a failure.
* Solved families do not need re-deriving: if `src/` already has the unit, its
  bytes are settled. The log's §9-§13 are history, not a to-do list.

Volatile used for a stack spill is a match-shape hack, not the original source. Mark those lines
`/* match-shape: volatile only forces retail's stack spill */`. Volatiles on hardware registers
(`extern volatile`, or an MMIO cast) are genuine and stay unmarked.

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
| A local `volatile` exists only to force a spill | keep it, and tag the line `/* match-shape: ... */` so cleanup can find it | 2.65 |
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

Each has a body and a diff summary in `tools/match/walls.txt`.

## Where to look

* `notes/mwccarm-codegen-log.md` - dated evidence, superseded rules, per-unit
  bodies and the walls as they were found.
* `tools/match/walls.txt` - every parked unit, one line each, with the exact residue.
* `AGENTS.md`, `CONTRIBUTING.md` - the hard rules, the edit loop and the
  release procedure.
