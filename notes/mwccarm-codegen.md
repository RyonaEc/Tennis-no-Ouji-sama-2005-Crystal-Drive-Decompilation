# mwccarm codegen levers (ATOJ)

> The older root-level playbook now lives here as
> [`mwccarm-codegen-solved-families.md`](mwccarm-codegen-solved-families.md). It
> holds the solved-family case studies (the 21-function per-slot state machine,
> the `0x274` flag setters, the prior-art survey) that this file distils into
> levers.

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
  bands every recent hit was b56-only. Probing is a *classification* step: a
  second build that happens to reproduce the bytes is a hint about the shape,
  never a licence to add a `mwcc_versions.txt` entry and call the unit matched.
* If **no** build and no spelling reproduces the shape, suspect provenance
  before compiler policy: `python tools/match/sdk.py <mod> <addr> <size>` looks
  for the bytes in the prebuilt NitroSDK libraries. `func_0201fcc8` was
  `MTX_RotX33_`, SDK asm, and the "2.0-generation" story was wrong for a day.
* Work from the live queue, not a stored list: a unit is unmatched only if
  `src/<mod>/<name>.c` is absent. Symbol names come from `symbols.txt`
  (`0x02085ff8` is `func_ov002_0202085ff8`), never from the address alone.
* Read register *roles*, not numbers. Fix the **first** structural divergence,
  then colours; do not chase the diff count.
* Reference globals by symbol. An absolute address can compile to identical
  bytes and then fail to link.
* Park after about two spellings. Record the body and the exact residue with
  `python tools/match/walls.py add <mod> <addr> "<reason>"` (the index line goes
  to `walls.txt`, the note to `walls-detail.txt`), then move on; a wall is a
  result, not a failure. Read one back with `walls.py show <mod> <addr>` rather
  than loading either file.
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
| Bitfield read uses `asr` | declare the member `s32 b : 1`; `u32` emits `lsr` | 2.59, 2.83 |
| A `mov r1, #0` appears before a call that should pass a field | pass the bitfield value itself as that argument; the `asr` result already sits in that register | 2.83 |
| An argument is copied to a scratch register at entry | the callee takes more arguments than the visible signature says; raise its arity | 2.83 |
| Tail call appears as `ldr ip, =callee; ...; bx ip` | write the call in tail position | 11.2 |
| `mov r0, #0; bx lr` | `return 0;` | 11.1 |
| Same body resists both compilers in the 68-88 B band | check the b56 address clusters before rewriting again | 2.46, 2.52, 2.63 |
| Candidate byte-matches but the link fails | absolute address where a symbol belongs, or a name/address mismatch | 2.64 |
| A failed compile looks like a match | treat an empty object as no match and recompile | 2.35 |
| Address hoisted into a callee-saved register where retail remakes `add` per use | declare the base `u32` and add the offset as an integer; cast to a pointer only at the dereference | 11.25-base |
| `mul` result and base pointer swap registers, frozen across spellings | the same `u32`-base lever | 11.25-base |
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
* **SDK asm blocks**: a retail `movs rX, #1` / `lsls rX, rX, #n` pair storing
  e.g. `0x1000` is hand-written thumb asm from a prebuilt library, not C. Every
  1.2 build (base, b56, sp2, sp2p3, sp3, sp4) materialises that store from a
  literal pool plus a trailing `nop` - 4 bytes longer - and 20 source spellings
  do not move it, because the constant is not the compiler's decision at all.
  `func_0201fcc8` is NitroSDK `MTX_RotX33_`; written as the SDK's own `asm void`
  block, b56 assembles it byte for byte with no override. Check with
  `python tools/match/sdk.py <mod> <addr> <size>` before parking one of these,
  and `sdkmap.py` for a whole module: 34 unmatched arm9 functions are SDK
  library code, not game code (`notes/nitrosdk.md`).
* **BIOS SWI stubs**: 4-6 byte wrappers whose object compare can pass while the
  linked ROM does not.

Each has a body and a diff summary in `tools/match/walls.txt`.

## Where to look

* `notes/mwccarm-codegen-log.md` - dated evidence, superseded rules, per-unit
  bodies and the walls as they were found.
* `tools/match/walls.txt` - every parked unit, one line each, with the exact residue.
* `AGENTS.md`, `CONTRIBUTING.md` - the hard rules, the edit loop and the
  release procedure.

## Address folds and stack temporaries (2026-11-22)

* **Blocking the pre-index fold.** `u32 *p = (u32 *)(base + A + B);` makes mwcc fold the add into
  the first store (`str r0, [rBase, rOff]!`). Retail often computes the address with a plain
  `add` first. Splitting the pointer across two statements and two variables restores it:
  `u32 *p = (u32 *)(base + A); u32 *q = (u32 *)((u8 *)p + B);` — the 72-byte
  `func_ov002_02052608` matches with exactly this shape.
* **A zero that retail spills.** When retail has `str r2, [sp]` / `ldr r0, [sp]` around a call
  argument, the source kept a zero in a stack local rather than passing a literal. Forcing the
  local to memory reproduces it: declare `u32 zero = 0;` and pass `*(u32 *)&zero`. That also
  creates the frame retail has (`stmdb sp!, {lr}` / `sub sp, #4`) and stops mwcc turning the last
  call into `bx ip`; a plain `func(0, ...)` compiles to a 52-byte tail call instead of the
  76-byte unit.

* **Big offsets split differently depending on how they are spelled.** A *struct member* offset
  above 12 bits is split by mwcc into `add rX, rBase, #high` plus a small immediate, so
  `typedef struct { u8 pad[0x30048]; u16 f; } Big;` gives `add r0, r2, #0x30000` + `ldrh r0,
  [r0, #0x48]`. The *same* address written as raw pointer arithmetic, `(u16 *)(b + 0x30048)`,
  becomes a pooled constant plus a register add (`ldr r0, [pc]; add r1, r2, r0`). Retail
  `func_ov002_0204d488` contains both spellings side by side, and that mixed source is what makes
  it match — one spelling alone collapses to a single address form and lands 8 bytes short.

* **A small `switch` keeps a branch that the equivalent `if` chain gets if-converted away.** For
  `func_ov002_020e4450` the natural `if (mode == 1) ... else if (mode == 2) ...` compiles both
  bodies to predicated `ldreq`/`orreq`/`streq` and lands 4 bytes short. Writing the same logic as
  `switch (mode) { case 0: break; case 1: ...; case 2: ...; }` gives retail's shape: `cmp r1,#1;
  beq` to a shared trailing `ldr/orr/str/bx lr` block plus one predicated case. Switch dispatch
  suppresses the conversion that `if`/`else if` triggers.

* **A chained `||` test gives one shared failure exit; separate `if (fail) return 0;` lines do not.**
  Four bounds checks written as four early returns each get if-converted inline (`movlt r0, #0;
  bxlt lr`) and cost 12 extra bytes. Writing `u32 ok = 1;` first and then a single
  `if (a < lo || a > lo2 || b < hi || b > hi2) ok = 0; return ok;` produces retail's shape: the
  flag is set in `r0` before the compares, every check branches to one shared `mov r0, #0; bx lr`
  block, and the success path is a single `bxle lr`.
* **Swapping and negating two values needs exactly one statement each.** `t = hi2; hi2 = -hi;
  hi = -t;` gives retail's four instructions (load, negate, store, negate). Adding a redundant
  `hi = -hi;` to "make the intent clear" makes mwcc negate the same register twice, spending one
  instruction and 4 bytes.

* **The `ip`/`lr` pair is a real tie, and it repeats across units.** In the u16 zero-fill skeleton
  (`mov rX,#0; mov rY,rX; mov rZ,#0x200; add <ptr>, base, i, lsl #1; ... strh`) retail puts the
  loaded base pointer in `ip` and the stored zero in `lr`; mwcc b56 does the opposite in every
  spelling tried, including the ov003/ov005 members of the same family. Because the instruction
  stream is otherwise identical, this is an allocator colouring difference, not a transcription
  error: statement order (permute.py over both regions), pointer type, declaration order, an
  explicit `u16 zero` local, and all 25 vendored compilers leave it. Do not spend impressions
  re-deriving these two bytes.

- **A type mismatch is a hard error, not a warning.** Assigning a `u8 *` to a `u32 *` makes
  mwcc 1.2/b56 emit `Error: <string not found>` and produce no object at all. `fastmatch.compare`
  then silently returns `False`, and `var3.py` prints `got 0 want N` — always read the byte
  count before believing a `no`.
- **`var3.py` matches through a relocation.** `fastmatch.compare` compares instruction text, so a
  pool word that still carries an undefined-symbol relocation is shown as `ldr r1, [pc, #..]` and
  matches retail no matter which symbol the source named. Two functions in this batch reported
  `MATCH` from `var3.py` and then failed `reg.py`, because the source pointed at `data_0204aec0`
  while retail's pool word was `0x02048d3c`. Always read the retail pool word out of the image
  (`fastmatch.retail`) instead of copying the data symbol from the neighbouring function, and
  treat `reg.py` — which links the ROM and checks the digest — as the only acceptance test.

- **Local declaration order assigns the callee-saved registers.** In `func_0202f118` the
  `func_0202e2ec()` base, a "default 1" counter and a loaded `u16` all compete for `r4`-`r6`.
  Declared `u8 *h; s32 m = 1; ...; u16 n;` mwcc gives the base `r6` and the `u16` `r4`; moving
  the `u16 n;` declaration to the top of the list gives retail's `r6` for the `u16` and `r4`
  for the base, with everything else unchanged. Reorder declarations before reaching for
  statement permutation.
- **Masking a `u8` parameter into its own local splits a load/increment merge.**
  `func_02036650` (strchr-style) reads the first byte and bumps the pointer, and mwcc fused
  the two into `ldrb r2, [r0], #1` in every spelling that used the `u8` parameter directly at
  the compare. Taking the mask once up front - `u8 cc = (u8)c;` - puts `and r1, r1, #0xff`
  between the load and the `add`, which breaks the fusion and reproduces retail's separate
  `ldrb r2, [r0]` / `add r0, r0, #1` entry, closing the unit at 60 B.

- **An array that is a struct field reproduces retail's scaled indexing; a raw base+offset
  expression does not.** `func_020255b8` scans 16 words at `data_02048d3c + 0x14`. Written as
  `*(u32 *)(data_02048d3c + 0x14 + i * 4)` mwcc keeps a byte cursor and emits
  `add r3, r3, #4` / `ldr r1, [r2, r3]`, which is 44B against retail's 48B. Declaring
  `typedef struct { u8 pad[0x14]; u32 arr[16]; } S; extern S data_02048d3c;` and writing
  `data_02048d3c.arr[i]` gives retail's `add r1, r2, r0, lsl #2` / `ldr r1, [r1, #0x14]`
  exactly, closing the unit at 48B.

- **Callee-saved colouring follows assignment order, not just declaration order.** In
  `func_020181b0` the same three locals written as declaring-with-initialisers
  (`u8 *b = a0 + 0x24; u32 t = *(u32 *)b; u8 *p;`) put `b` in r5 and `t` in r4, the reverse of
  retail. Declaring `u32 t; u8 *b; u8 *p;` and assigning in statements (`b = a0 + 0x24;
  t = *(u32 *)b;`) gives retail's `add r4, r0, #0x24` / `ldr r5, [r4]` exactly.

- **The comparison's signedness picks `ge` versus `hs` on the same code.** `func_02008178`
  doubles a saturating counter and bails at a limit. With `u32 v` the guard assembles as
  `ldrhs` / `bxhs`; declaring `s32 v` and comparing `v >= 0x3b9ac9ff` gives retail's
  `ldrge` / `bxge`. Everything else in the function is identical, so a one-word predicate
  difference is the whole residue when a limit check will not close.

- **A `Pair` struct copy is what folds two stores into `[rN]` / `[rN,#4]`.** In `0x0202ad9c`
  eight stores to literal hardware addresses (`0x04000290`, `0x04000294`, ...) compile to eight
  pool loads and eight stores (132B). Writing them as three copies of `typedef struct { u32 a;
  u32 b; } Pair` (`*(volatile Pair *)0x04000290 = *(Pair *)s;`) gives retail's five pool loads
  and the structured `str x,[r2]` / `str x,[r2,#4]` pairs, exactly 108B. Use a struct copy, not
  two scalar stores, whenever retail loads both halves before storing either.

- **A u32 temporary keeps the case-fold mask off the value.** Converting with `u8 c` and writing
  `c = c + 0x20` re-truncates and inserts `and r2, r2, #0xff` before the `orr`. Loading into
  `u8 c` but converting into a separate `u32 d` (`d = c - 'A'; d = d <= 25 ? d + 'a' : d + 'A';`)
  drops the mask and if-converts to retail's `sub/cmp/addls/addhi` with no branch.

- **A one-block conditional tail is always inlined, never branched.** mwcc 1.2/b56 turns
  `if (f() != 0) { ... }`, an early `return`, and `goto end` into `popeq`/`bxeq` when the body is
  the last block. Retail's `beq` to a shared `pop`/`bx` epilogue cannot be spelled from a single
  trailing block; park the unit instead of hunting for a branch that the compiler never emits.

- **The data symbol, not the literal address, decides the base pointer's register.** In
  `0x020208b0` writing `u16 *p = (u16 *)0x02048c74;` and indexing `p[0xc]` leaves the base in ip
  where retail keeps it in r1 and everything else falls into a different rotation. Declaring
  `extern u16 data_02048c74[];` and writing `data_02048c74[0xc]` gives retail's `ldr r1, [pc]`
  base immediately; the same body then differs only in the temporaries' numbers.

- **A short `if` body is if-converted unless it is the `else` arm.** A four-instruction
  `if (a0 == 0) { ...bit clear... } else if (a0 == 0x100) { ...set... }` compiles to five
  predicated instructions plus a skip branch (`ldreq`/`biceq`/`streq`/`beq`). Inverting the outer
  condition — `if (a0 != 0) { if (a0 == 0x100) {...} } else { ...bit clear... }` — restores
  retail's `beq` into a real out-of-line block and the extra `b` that closes it.

- **A word built from several shifted fields needs one assignment per sub-word.** In
  `0x02027080` the single expression `a4 | (a1 << 24) | (a3 << 26) | (a9 << 16)` is 8B short of
  retail: mwcc folds each `lsl` into the following `orr`'s shifter operand. Writing

      w1 = (a3 << 26) | (a1 << 24);
      w1 |= a9 << 16;
      w1 = a4 | w1;

  makes mwcc materialise each shift with its own `lsl` and reaches retail's exact size. When a
  packing routine compiles short, count the `lsl`s: retail usually has one per field.

- **A counter updated in place needs its address in a pointer variable.** In `0x0201779c` the
  tail `*(u16 *)(node + 8) = *(u16 *)(node + 8) - 1;` lets mwcc fold the offset into both
  accesses (108B). Declaring `u16 *cnt = (u16 *)(node + 8);` and writing `*cnt = *cnt - 1;`
  forces retail's `add r1, r0, #8` / `ldrh` / `sub` / `strh` (112B). The same holds for any
  in-place u16 read-modify-write that retail materialises an address for.

- **When var3 prints `bl` differences, read the rest of the diff.** Calls to not-yet-linked
  siblings differ in the object by definition, so `bl` lines are noise; but var3 prints the
  whole hunk list, and a register swap further down is the real residue. Truncating the diff
  at the first hunk hid a five-word ip/lr swap in `0x0201780c`.

- **Split steady-state updates into per-field statements when the test reads one of the results.**
  `data.f18 += k; data.f1c += k; data.f20 -= k; if (data.f20 == 0) { long path; return; }
  func_0202d9f4();` produces retail's field-order loads and the `r3`/`r2`/`r1` mapping; hoisting
  into locals first makes mwcc pick a different order and register set. The `== 0` spelling also
  puts the long block in the fall-through and keeps the short one at the tail.

- **Masked reads may be bitfields.** `(ldrh) + lsl #29 + lsr #29` is a `u16 x : 3` field and
  `(ldrb) + lsl #28 + lsr #28` is a `u8 x : 4` field; writing `v & 7` folds to one `and` and the
  shift pair never appears. Give the register block a struct type with the bitfields at their real
  offsets and assign them to the destination byte-by-byte.

- **A cached-read function wants one return, not two.** `if (x != -1) return x; ... return x;`
  if-converts the early exit (`popne`/`bxne`) and duplicates the epilogue; writing the guard as
  `if (x == -1) { ...compute and store... } return x;` leaves a single return and retail's shared
  tail. Both spellings can be the same size, so check the shape, not the length.

- **Verify pool constants by bytes, not by eyeballing the hex print.** A wrong digit in a pooled
  literal leaves `var3.py` at `MATCH`-minus-one-pool-word and only `reg.py` reports the miss.
  Compile with `compile_text`, diff against `retail`, and look for differing words outside the
  relocated ones.
- **`(i * 3 + 2) * 4` is not `i * 12 + 8`.** The factored form gives `mov #3; mul; add; add lsl`,
  the flat form gives `mla` against the pooled base.

- **A `while` loop that retail enters with `b <bottom>` can be forced with an explicit rotation.**
  `goto test; body: <body>; test: if (cond) goto body;` compiles to the single entry branch plus a
  bottom test, where `while (...) {...}` gives mwcc a pre-test with an if-converted early exit.
  Treat the goto form as a codegen workaround, not as the original source shape.

- **Stack-slot order follows the declaration order of the locals.** When retail reads its result
  pairs from `sp+0` and its input descriptor from `sp+8`, declaring the output array before the
  input struct is what places them; the reverse swaps the slots and every offset in the diff.
- **Pad fields must have the pad's own element type.** `u32 pad0[0x20]` pads 0x80 bytes, not 0x20;
  the unit stays the same size and only the field offsets move, so check the immediate offsets.

- **Predicated result pairs follow the tested condition's polarity.** `x == 0 ? 3 : 0` compiles to
  `mov r0, #3` plus `movne r0, #0`; retail's `movne r0, #0 / moveq r0, #3` comes from writing the
  branch on the non-zero case (`if (x != 0) return 0; return 3;`). Same size, two words apart, and
  `var3.py` may report only `bl` differences — confirm with a compiled-vs-retail word dump.

- **A local `T *p = &global;` at the top of a function hoists the global's address load above an
  early return.** Referencing `global.field` directly keeps the materialisation at first use, after
  the guard. Same size, different order — and this one is common in init/latch functions.

- **Repeated `ldrh`/`strh` to the same stack slot needs a `volatile` local.** Three identical
  hardware reads whose values are discarded only survive if the destination is
  `volatile u16 tmp;` — otherwise mwcc eliminates the dead stores.
- **Check `relocs.txt` and `symbols.txt` before assuming a pooled word is a symbol.** Silence in
  both means the word is a literal address (`0x04000204`, `0x09fe2ffe`).

- **`lsl #30` + `asrs #31` is a signed one-bit bitfield at bit 1, not `v & 2`.** Model the word as a
  union of `u32 w` and a bitfield struct (`u32 b0 : 1; s32 f : 1; u32 rest : 30;`), read the field
  for the test and use `flags.w` for the read-modify-write, which keeps retail's add/ldr/bic/str.

- **A read-modify-write through a fresh address variable gives the pre-indexed form.** `u8 *c = p + 0x34; *c = *c - 1;` compiles to `ldrb r1, [r2, #0x34]!` + `strb r1, [r2]`, not to separate add/load/store.
- **The overlays are ordinary arm9 code.** `ov002`..`ov010` use the same idioms and pool layout; the matching tools take the module name (`udis.py ov008 0x0204ce20`).

- **`u8 >> n` gives `asr`, not `lsr`.** The promotion to signed `int` makes the shift arithmetic; read the byte into a `u32` local first when retail shifts logically.
- **Version probes rarely help a register tie.** 1.2/b56 and 1.2/sp2 allocate the same registers for a given source shape, so a swapped-register residue is a spelling problem, not a build problem.

- **A guarded post-increment `(*(T *)(p + k))++` rematerialises the address.** Writing it through a cached pointer gives three instructions where retail has four (`addne r2, r0, #k; ldrhne r1, [r2]; addne r1, r1, #1; strhne r1, [r2]`).
- **Sibling families pay off in the overlays.** Two-byte-adjacent constants (a pool word one nibble apart, a different return value) are the only differences between members, and each member needs its own verification run.

- **Overlay callees are `func_ovNNN_<addr>`.** A relocation into the same overlay resolves against the prefixed symbol; using the bare `func_020xxxxx` name that `udis.py` prints gives an undefined-symbol link error that `var3` cannot see.
- **A pooled `0xffff` add means the source casts to u16 before adding.** `(u16)(n + 0xffff) > 1` becomes `add r0, r1, r0, lsr #16` with 0xffff in the pool; writing `n - 1` compiles to `sub` instead.
- **A trailing `mov r0, #1` before `bx lr` means the unit returns 1.** A `void` spelling comes out one instruction short.
### 3.13 Nested-index fusion (2026-11-22)

Theme: `b + j * K + i` inside a nested loop. mwcc 1.2/b56 always folds the whole
sum into ONE accumulating address register and increments that, so the inner body
carries a single `add rX, rX, #1` and no per-iteration base add. Retail keeps the
row base and `i` in separate registers and forms `add r0, rowbase, i` every
iteration, then advances the row base by K at the end of the outer body.

Evidence: `ov002 0x0205fe04` (140 B). The direct spelling
`b[j*3 + i + 0x21e]` / `b[j*3 + i + 0x224]` compiles to 144 B, one instruction over,
because mwcc accumulates the address; giving the row base its own advancing
pointer `p = b; ... p += 3;` instead compiles to 176 B, because mwcc then keeps two
live bases and unrolls the constant-trip outer loop. Casting the stored `i` to
`u8` changes nothing (a `u32` store to a `u8` lvalue already gives `strb`).
### 3.14 Base-plus-field load must not be an absolute expression (2026-11-22)

Theme: `*(u32 *)(0xBASE + FIELD)` folds the sum into a single pooled literal, so
the unit loses retail's `ldr rX, [pc]` + `ldr rY, [rX, #FIELD]` pair. Writing the
global as a typed struct pointer keeps both:

```c
typedef struct { u8 pad[FIELD]; u32 p; } T;
u32 v = ((T *)0xBASE)->p;      /* ldr rX,[pc]; ldr rY,[rX,#FIELD] */
```

Evidence: `arm9 0x02032c70`. The absolute form pools 0x027fff7c and loads
`ldr ip, [r3]`; the struct form pools 0x027fff60 and loads `ldr ip, [r3, #0x1c]`
exactly as retail does, worth 8 bytes on that unit and moving it from 124 B to
136 B against retail's 144 B.

## Session notes, 2026-11-23 (overlay teardown/init family)

- **A compound assignment hides the reload.** `*(volatile u32 *)0x04001000 &= ~0x1f00;`
  allocates the address register and the scratch the other way round from retail.
  Writing the load out explicitly, `*(volatile u32 *)0x04001000 =
  *(volatile u32 *)0x04001000 & ~0x1f00u;`, reproduces retail's `r3`/`ip` split
  (`func_ov009_0204ebf4`).
- **Spilled constant arguments come from `volatile` locals.** Where retail does
  `mov r0, #0 / str r0, [sp, #8] / ldr r0, [sp, #8]` before a call, the source kept
  the value in a `volatile u32` local. The frame is exactly one 4-byte slot per
  local, and the slot offset is 4 x the local's declaration index, so the
  declaration order is also how the slots are placed
  (`func_ov002_02060344`, `func_ov008_0204e084`: declaration order `b, d, a, c, e`
  gives the use order `a, b, c, d, e`, slots `8, 0, 0xc, 4, 0x10`).

## Taking a double parameter's address

When retail's double argument is spilled by `push {r0, r1, r2, r3}` and then read
back from `[sp, #8]` / `[sp, #0xc]`, the source took the parameter's address. The
fdlibm macros do exactly that - `#define HI(d) (*(1 + (s32 *)&(d)))`,
`#define LO(d) (*(s32 *)&(d))` - and reproduce the prologue. A union-based HI/LO
that only copies the double leaves the argument in registers and comes out short
(`func_02036874`: 180 B against 192 B). `func_0203684c` was parked as an
"unreachable prologue, 8-byte struct + 2 unused u32 args" wall until the same
macros produced its 40 bytes exactly; treat that shape as the first thing to try
on any small libm wrapper, and on any older wall note that complains about an
unexplained `push {r0-r3}` beside a double.

## Frame layout and the limits of if-conversion (2026-11-25)

Axes, not matches - every one still needs a compiled byte-diff and `ninja sha1`.

* **The frame is a chain, then a spill pool, then aggregates.** Locals that keep
  a stack slot form a *chain* in declaration order; compiler temporaries (and
  locals coalesced into them) form a *spill pool* below it; arrays/structs go
  above both. The chain's length sets where the pool starts, so **one surplus
  chained word displaces every pool slot** - a single 4-byte surplus was worth 44
  wrong slot references in the studied function. Read the two regions apart with
  `slots.py`: a local with a `DW_AT_location` is in the chain, one without is
  pool-or-register. The lever: **a second definition of a local in a different
  block moves it out of the chain into the pool** (on the next line it is a dead
  store and nothing moves). Absence of a DIE is not evidence a local left the
  frame - it can keep its slot and lose its location.
* **b56's materialisation regime.** Under `2004/b56` a member RMW materialises
  its base by default when both sides are spelled *identically*
  (`p->f = p->f | x`), and folds the offset when the two sides differ textually;
  a cv-qualified cast is its own CSE class. Our materialised-RMW walls were
  solved with a pointer local (`u32 *q = (u32 *)(p + 0x24); *q |= k;`), which is
  this effect reached a different way. The 64-bit laundering form
  (`*(int *)(((int)base + OFF) & 0xFFFFFFFFFFFFFFFF)`) is a third route; it is
  LICM-hoisted out of loops unless paired with `#pragma opt_loop_invariants off`,
  and value-numbers into one register unless paired with
  `#pragma opt_common_subs off`. We tried the launder alone before; the pairing
  is the part we had not.
* **cond-opt's refusal rule.** It will not if-convert an arm whose
  jumped-over region itself contains a conditional region (another branch or
  predicated pair); the fold budget is ~7 instructions, not 5. Two consequences
  we can use: *inverting an if-arm's condition defeats full if-conversion*, and
  a predicated select (`mov r0,#A; movne r0,#B`) comes from a
  **default-then-override** statement pair, not from a ternary. This is the
  closest thing we have to an explanation for the parked predicated-pair walls
  (`func_ov004_020623e0`, `func_ov002_020f2970`), where mwcc insists on
  predicating the `eq` arm.
* **The edit loop costs 0.16 s per candidate** (measured 2026-11-25, three
  `var3.py` runs: 0.16 s each). At ~6 candidates/second the correct default is
  an exhaustive sweep over a hypothesis, not more thinking between compiles -
  and a "dead lever" result is scoped to the current structure, so re-sweep
  after any restructuring.
* **Tooling trap: unknown pragmas are silently ignored.** A misspelled pragma
  reads as "inert", not as an error, so a pragma experiment only counts if the
  name is one mwccarm actually honours; `pragmas.py` already sweeps the
  vocabulary we trust.
* **A switch is not an if-chain with sugar.** For a small `if (x==0) … if (x==1) …`
  chain over a loaded byte, b56 is free to reorder the arms - it will test the
  *body's* condition first when one arm's body is a constant compare (seen in
  `func_ov002_02064cb0`, where the chain gave 164 B and reordered
  `cmp r0,#0; bne` above the state test). The same arms written as a
  `switch (v)` emit retail's shape: **one load, then a pure `cmp`/`beq` chain**
  in case order. Prefer `switch` whenever retail shows a single load feeding a
  chain of compares - and when two adjacent cases have byte-identical bodies,
  write them as two separate case groups (mwcc emits both copies, as retail
  does in `func_ov003_020d5948`); merging them into one `case 7: case 8:` group
  silently costs 6 instructions (168 B -> 144 B).
* **A value read in two arms: duplicate the load, do not share a local.** With
  `id = *(u8 *)(base + 0x106);` hoisted before `if (a2 == 0) … else …`, b56
  pulled the load *above* the branch and saved an instruction retail spends
  reloading it per arm (`func_ov002_02060688`: shared local 164 B, load written
  out in both arms 168 B = match). If retail loads the same byte once per arm,
  repeat the full load expression in each arm.
* **Integer base keeps a register-offset store un-folded.** Marking the bss base
  `u32` (not `u8 *`) is what stops b56 folding `base + off` into an `add`:
  `func_ov002_02080ce8` wanted `lsl r2, r0, #2` followed by
  `strlt r7, [r6, r2]`; with `u8 *tbl` we got `add r2, r6, r0, lsl #2` /
  `strlt r7, [r2]` instead (10 diffs, same size). Declaring `u32 tbl = base + …`
  and casting only at the dereference reproduces the register-offset store
  exactly. Same family as the u32-base lever above, now confirmed for stores as
  well as loads.
* **Jump-table width comes from the case labels, not the case bodies.** When
  retail shows a wide table (`cmp rX,#N; addls pc,pc,rX,lsl #2` covering values
  that are all `default`), b56 only builds that table if the *source* names every
  value in the range. Two cases at 1 and 7 with a `default:` compile to a
  `cmp`/`beq` chain (`func_020151a8`: 152 B vs retail 168 B); adding
  `case 0: case 2: case 3: case 4: case 5: case 6: break;` produces retail's
  8-entry table byte for byte. Same for `func_ov003_020d5948`, which needed
  cases 0-5 spelled out to get its 10-entry table. **The case-body order in
  memory is the reverse of the source order**: retail's blocks ran 8/9 then 7
  then 6, so the switch has to be written `case 8: case 9: … case 7: … case 6:`
  to land them in that order. Two adjacent cases with byte-identical bodies still
  need separate case groups (merging them costs the second copy).
* **Split the index, do not add it: `p[a2]` after `p = &tbl[k]`.** For a
  two-part index into a data table (`tbl[9*a1 + a2]`), b56 is free to compute
  `9*a1 + a2` and use the symbol as the addressing base, or to accumulate
  `symbol + 9*a1` and use `a2` as the index register. Retail did the latter
  (`func_ov002_0207fcf0`). Writing `u8 *p = &data_X[a1 * 9]; t = p[a2];` forces
  the second form; the flat `data_X[a1 * 9 + a2]` gives the first (20 diffs,
  same size).
* **Unsigned parameters widen the branch conditions.** The same switch over
  `a1` emits `bhi`/`blo` with `u32 a1` and `bgt`/`blt` with `s32 a1`, and the
  final `return a0 >= t;` emits `movhs/movlo` versus retail's `movge/movlt`.
  Declaring the parameters `s32` when retail uses signed conditions fixed the
  last 12 diffs of `func_ov002_0207fcf0` in one edit.
* **Re-read the field for the delta; keep only the mask.** For
  `m = g[i] & MASK; if (m == x) { off = g[i] - m; … }`, b56 will happily keep
  `g[i]` in a register from the test and compute `off` from it, one instruction
  short of retail. `func_0202db70` wants the *second* read (`ldr r2, [r5, #0x18]`
  again after the branch) with the masked value reused from the test: write
  `u32 m = g[6] & -0x200;` and then `off = g[6] - m;`, not
  `v = g[6]; m = v & -0x200; off = v - m;` (160 B vs 164 B).
* **A struct view beats a byte offset for a base + const + scaled index.** When
  retail addresses `base + 4 + i*2` as `add r0, base, i, lsl #1` plus a `+4`
  load offset, do not write the arithmetic out: `p + i*2 + 4` in bytes makes b56
  strength-reduce `i*2` into a second induction variable (an extra callee-saved
  register *and* a stack frame: 200 B vs 168 B for `func_0201df30`), and a
  `u16 *` view drops the accumulator but folds the 4 into the index
  (`add r0, r5, #2; lsl; ldrh` = 172 B). Declaring the record shape -
  `typedef struct { u32 f; u16 w[4]; } Slot;` and reading `p->w[i]` - gives
  retail's `add` with a shifted register operand plus the member offset as the
  load immediate, byte for byte.

- Loop shape follows where the condition lives. A `while` whose header holds the whole
  condition expression (a nested ternary included) is rotated by b56: `mov n,#0; b test;
  body; test: <cond>; bne body` — one load site, body before the test. Hoisting the same
  condition into a local and looping with `while (1) { c = ...; if (c == 0) break; ... }`
  puts the test first and predicates the body (`addne/addne; bne head`): same length,
  different layout. An `if (cond) do {...} while (cond);` guard duplicates the body.
  `arm9 02032c38` matched only in the first spelling.
