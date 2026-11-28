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

- b56 will not inline a plain `static` helper - it emits a call for it (a helper called
  three times from one loop turned 340 B of straight-line code into a 152 B function plus
  three `bl`). Only `static inline` is inlined. When a routine shows the same block of
  code three times and no `bl` anywhere, the original used an inline/macro, not a static
  function.

- A conditional sign flip is a statement, not a ternary. `if (c) v *= -1;` emits
  `mvn{cond} r0, #0; mul{cond} r4, v, r0` (with an unconditional `mov` of the
  other arm), while `v = c ? v : -v;` and `v = c ? -0x200000 : 0x200000;` collapse
  the multiply into `movne`/`rsbeq` and come out 12-16 B short. Measured on
  `ov002 020a038c`: the ternary form is 172 B, the statement form reproduces
  retail's two-instruction idiom exactly.

- `x % 2` and `x & 1` are different code. On a signed `x`, `x % 2 == 0` emits the
  four-instruction `lsr #31 / rsb / adds / ror #31` sequence (mwcc's signed
  modulo); `x & 1` emits a single `ands r0, r0, #1` and loses 12 B. If retail
  has the rotate sequence, the source wrote `%`. The value can still be built by
  an unsigned shift — `s32 n = (u32)*(u8 *)(p + 0xe5) >> 1;` keeps `lsr` and
  only the test is signed.

- A field that is read or written on both sides of a call needs its own pointer
  local. `*(u32 *)(b + 0x5c) += f();` recomputes `b + 0x5c` after the call and
  costs a register and the frame pad (180 B); `u32 *q = (u32 *)(b + 0x5c);` held
  across the call is what produces retail's extra push and `sub sp, #4` (188 B).
  Declaring it earlier in the block does not change when the scheduler emits the
  `add` — source order is not the lever there.

- `(x / 2) * 2` and `(x & ~1)` are not interchangeable. The mask folds to a
  single `bic r0, r0, #1`; the division spelling keeps retail's
  `lsr r0, r0, #1` + `lsl r0, r0, #1` pair and is 4 B longer. Measured on
  `ov002 020bd5e8`, which is 4 B short in both mask forms. Rewriting the same
  value as `(x >> 1) << 1` does not help - b56 re-folds it - so the division has
  to be written as a division.

- A `switch` whose default is the same value as the trailing `return` merges both
  into one block, and the size drops with it. Writing the case out explicitly
  (`case 2: return 1;` before `}` plus a trailing `return 1;`) keeps the separate
  return block that retail has. Measured on `ov002 02063e58`: 232 B when merged,
  256 B with the explicit case, and 260 B once the inner test is ordered so the
  non-negated branch is the one that returns.

- **A store before a guard can be 4 bytes cheaper than after it.** When a function
  stores a constant and also returns it on an early exit, writing the store *first*
  lets the two paths share one `mov rN, #const`; writing the guard first makes mwcc
  emit the return value predicated (`moveq`) plus a second materialisation. The
  compiler may sink the store past the guard as long as the guard does not read the
  stored field, so the source order is free to be the cheaper one. Measured on
  `ov002 0209d584`: 264 B guard-first, exactly 260 B store-first.

## Direction byte: `lsl / sub / rsb`, not one `rsb`

Both ov002 angle-driving families set a direction from the entity's byte 0x2e6
(`b`), take `half = b >> 1` and a direction of `1 - 2*half`. Two spellings:

    s8 dir = 1 - (b & 0xfe);        /* rsb only: one instruction   */

Retail emits three:

    lsl  r2, r3, #1                 ; half * 2
    sub  r2, r2, #1                 ; half * 2 - 1
    rsb  r2, r2, #0                 ; -(half * 2 - 1)

which is the source

    s8 dir = -(half * 2 - 1);

Symmetric in value, two instructions apart. Writing it as `1 - (half << 1)`
still folds, so keep the negation on the outside of the multiply. This alone
took ov002 0x0208f38c from 596 to 600 of 608 and 0x0208f860 from 732 to 736 of
756 - check it first on any sibling of these families.

## Signed cast of a call result: `add r0, r0, #0` before the shift pair

Retail both families contain

    bl   #0x2053468
    add  r0, r0, #0
    lsl  r0, r0, #0x10
    asr  r5, r0, #0x10

after `func_ov002_02053468`, whose extern returns `u32`. The `add #0` is a real
emitted instruction, not a disassembler artifact. It is the **`u16` truncation**:

    ang = (s16)(u16)func_ov002_02053468();   /* add r0,r0,#0 then lsl/asr  */

`(s16)call()`, `(s16)(s32)call()`, a `s32` temporary and an `s16` extern all
fail to reproduce it (the last is worse by a whole instruction). The double cast
was found by respell plus a four-way batch: it took ov002 0x0208f38c from 600 to
608 of 608 on the instruction stream, and 0x0208f860 from 736 to 744 of 756.
Reach for it on any `u32`-returning call that is truncated to a small signed
type.

## CSE across calls: keep the two argument trees different

b56 will fold a repeated address computation in two consecutive call
arguments, but only when the arguments are the *same* expression tree:

```c
func_a((u32)(base + 0x100), 0xb980);          /* add r0, r5, #0x100 */
func_b((u32)((u16 *)base + 0x80), 0x1900, 0xb980);  /* add r0, r5, #0x100 */
```

Written as `base + 0x100` both times, one of the two is hoisted into a callee
saved register and reused (`add r6, r5, #0x100`, then `mov r0, r6` twice) - one
instruction shorter than retail, and the residue looks like a scheduling tie.
Spelling the second argument through a different type keeps the trees distinct
and reproduces both adds. Same lesson as the pooled-literal case: an
`add #imm` that retail repeats is evidence the source recomputed, not that it
saved.

Two symmetric loop counters (`i`, `j` in a nested pair) are coloured by
declaration order: whichever is declared first is not automatically the outer
one. When only `lr`/`r6` are swapped across the whole loop, a declaration
shuffle with the initialisers split out of the declarations is the lever, and
`permute.py` finds it in seconds where `respell.py` plateaus.

## Tail-test loops: put the condition in one `while`

A table walk written as `while (p != 0) { if (*p == 0) break; (*p)(); p++; }`
produces two predicated exits and its own `pop`/`bx` pair. Folding the null
test into the loop condition -

```c
Fn f;
while (p != 0 && (f = *p) != 0) {
    f();
    p++;
}
```

- gives retail's rotated loop: one `cmp`/`popeq` at the top, `blx` in the body
  and a single shared epilogue. One instruction saved, and the only shape that
  matched on `arm9 0203b698`.

## Reading `% n`: the shift amount is the modulus

mwcc expands `i % n` (unsigned) as
`lsr rA, i, #31` / `rsb rB, rA, i, lsl #(32-k)` / `adds rC, rA, rB, ror #(32-k)`,
where the mask is `2^k - 1`. So `lsl #31` / `ror #31` is `% 2` and
`lsl #30` / `ror #30` is `% 4`. Read the shift before assuming the divisor; one
step of k is one instruction's worth of difference in the loop body.

## `(u32)&local` is hoisted, a pointer parameter is not

Passing a local's address as `(u32)&c` makes mwcc materialise the frame address
once in a callee-saved register and pass the copy; retail recomputes
`add r0, sp, #0` before every call. Declaring the callee's parameter as
`void *` / `u8 *` and passing `&c` (or the array name) keeps the
rematerialisation, which is what frees the register retail uses for something
else. Same family of mistake as the byte-offset counter: the cast decides the
codegen, not the arithmetic.

## The shift in a magic division names the divisor

`umull r1, r0, x, 0xcccccccd` followed by `lsr r0, r0, #2` is `/5`; the same
literal with `lsr r0, r0, #4` is `/20`. Both use 0xcccccccd, so read the shift
before guessing: one step of shift is a factor of four in the divisor, and
writing the smaller divisor costs an instruction and breaks the match.

## `if (c) { ...; return 1; } return 0;` vs the `else` assignment

For a call result that both guards a couple of stores and becomes the return
value, the early-return pair and the if/else assignment are not the same bytes:

- `if (res) { store; store; return 1; } return 0;` emits `movne r0, #1` first,
  stores predicated on the same flags, then a second `cmp` for the exit;
- `if (res == 0) r = 0; else { store; store; r = 1; } return r;` emits
  `moveq r0, #0` then `movne r0, #1` then the predicated stores, with no second
  `cmp` - the flags of the one comparison are reused.

When retail shows the boolean materialised before the stores and no trailing
comparison, write the `else` assignment.

## `case 0: default:` shares a body and leaves a branch to the next instruction

When a switch's default does not get its own body, write the label that shares
the body explicitly:

```c
switch (v) {
case 0:
default:
    f(9);
    break;
case 1:
    f(0xa);
    break;
}
```

mwcc then orders the comparisons `cmp #0; beq case0; cmp #1; beq case1;` and
lets the fall-through reach the shared body, so the first branch targets the
instruction immediately after it. A plain `case 0:`/`case 1:` switch with an
empty default instead emits a `b` to the join after the two comparisons, which
is four bytes the retail body does not have. A `beq` whose target is the next
instruction is the signature of this shape, not a stray branch.

## One shared `return 0` behind per-branch early returns

Two branches that both fail into `return 0` share one out-of-line
`mov r0, #0; pop; bx` block in retail, and both guards branch to it. Write it as
the trailing statement, with the successful path returning early:

```c
if (mode == 1) {
    ...
    if (r != 0) return f(p, 8);
} else if (r != 0) {
    return f(p, 2);
}
return 0;
```

`if (r == 0) return 0;` inside each branch inlines the epilogue twice and costs
8 bytes; `if (r != 0) return ...;` plus one trailing `return 0;` produces the
shared block, the two `beq`s to it, and the `pop`/`bx` after each call site.

## A loop-body local's constant is hoisted into the frame, not folded

Constants that live in locals declared inside a loop body are loop-invariant to
mwcc: it stores them in the frame before the loop head and *reloads* them from
memory at every use instead of using `movmi rX, #0`. So `ldr` from a slot whose
content the same function just stored is not a sign of a struct - it is a local
declared inside the loop:

```c
for (i = 0; i < n; i++) {
    s32 lo = 0, bot = 0xbf;   /* hoisted: three stores before the loop head */
    ...
}
```

The init block sitting between the loop guard and the loop head, rather than at
the top of the function, is the confirmation. Literals used only in a
comparison stay in registers (`mov fp, #0xff`), so the frame holds only the
ones read as values.

## Variable `/` and `%` call `_s32_div_f`, which lives at 0x0203b0c0

mwcc emits `bl _s32_div_f` for a division or modulo by a run-time value, and the
ROM's own signed divmod helper (quotient in r0, remainder in r1) sits at
0x0203b0c0. That address is now named `_s32_div_f` in `config/ATOJ/arm9/symbols.txt`
instead of `func_0203b0c0`, so `%` in a unit links. Two consequences:

- The *operands must be signed*. An unsigned `%` compiles to `_u32_div_f`, and
  this ROM has no unsigned helper, so the unit fails to link with
  `Undefined : "_u32_div_f"`. Give the fields `s32` types (retail's signed `bgt`
  on the same fields is the confirmation) rather than casting to `u32`.
- Because mwcc reads the remainder out of r1, retail's index often comes from r1
  while r0 is overwritten by the next load; do not try to spell that with a
  struct return or a function-pointer call, which changes the frame.

## `((u32 *)p)[i >> 2]` keeps the shift, `*(u32 *)(p + (i >> 2) * 4)` folds it

Retail often shows `asr rX, rY, #2` followed by `ldr rX, [rBase, rX, lsl #2]`.
Written as a byte offset (`p + (i >> 2) * 4`) mwcc folds the pair into
`bic rX, rY, #3` plus an unscaled load, which is two bytes and one instruction
different. Indexing a typed pointer keeps the shift and the scaled access:

```c
*(u8 *)(dst + i) = (u8)(((u32 *)p)[i >> 2] >> ((3 - (i & 3)) * 8));
```

The same choice decides `blt`/`blo` and `asr`/`lsr`: the loop counter has to be
`s32` for retail's signed forms.

## `ldrb` vs `ldrsb` is the load's type, not the destination's

`a1 = *(s8 *)(r + 9)` emits `ldrsb`; retail often has `ldrb` with the value
landing in a `s32` parameter, which is `a1 = *(u8 *)(r + 9)`. Pick the load width
from retail's mnemonic and let the assignment widen to the parameter.

## Refresh the queue before matching, or the counter will not move

`true_fresh.txt` is a snapshot. Two units re-derived this pass (`func_02028888`,
`func_0202bea8`) already had sources in git: `cdiff` and `var3` reported IDENTICAL
and `reg.py` reported MATCH, but the totals did not change, because a matched unit
counted once stays counted once. Always run `python tools/match/fresh.py --write`
first; `git ls-tree HEAD src/<mod>/<name>.c` answers "is this already matched" in
one command.

## An SDK partial hit is usually a transcribable unit

`sdk.py` reporting 72/76 rather than 76/76 is normal for an SDK routine: retail
has its relocations applied, so the differing bytes are the pool word and the
`bl`. `func_0202564c` is `libos.a(os_context.o)` `OS_SaveContext`: transcribing the
SDK's own `asm void` block, with the struct offsets restated numerically
(CP_CONTEXT `+0x48`, CPSR `+0`, PC_PLUS4 `+0x40`, SP_SVC `+0x44`) and
`ldr r1, =func_0202ae08` for the `CP_SaveContext` call, assembled byte-exactly on
b56. For an SDK C function, restate the header's structs at the offsets retail
uses and compile the body unchanged: `func_0202bea8` is `PM_SetBackLightAsync`
(`spi/ARM9/src/pm.c`) almost verbatim, and `FSi_SeekDirCommand`
(`fs/common/src/fs_command.c`) needs only `FSDirPos {arc, own_id, index, pos}`,
`FSArchiveFNT {start, index, parent}` and `fnt +0x28`.

## A unit next to a matched `asm void` unit is often its sibling

`func_0203b50c` sits right after the matched `func_0203b4e4` and is the same
read-modify-write with the arguments repacked first:
`(x & 0x1f) | ((x & 0x1f0000) >> 8)` for each, then the shared bic/eor/str, then
the repacked previous value with `| 0x40000000`. Reading the matched neighbour
gave the whole calling convention, the push set and the pool layout; grep
`src/<mod>/` for `asm void` and check the addresses adjacent to those files.

## A constant that must not fold needs a symbol, not a literal

`func_02025e48` (`OS_GetInitArenaHi`) reproduces its switch, its jump table and
every arena boundary, but the DTCM arm needs `OSi_SYS_STACKSIZE` to be a
*nonymous* zero: retail materialises it from a pool word and keeps
`cmp r1,#0`, `sublt`, `subge` around it, i.e. the SDK's absolute LCF symbol whose
value is 0. Written as a plain `0` (or a `const u32 = 0`) mwcc folds the whole arm
to `return 0x027c0020` and 172 B becomes 104 B.

## 2.50 A NULL comparison does not compile in b56 (2026-11-26)

`if (param == NULL)` in a unit aborts the compiler with

```
func.c:12: <string not found>
```

- no line text, no diagnostic - and the message text is missing from the binary,
so it reads like a parser bug rather than a type rule. `if (param == NULL) return;`
fails the same way whatever the pointer type is; `if (!param)` compiles. The
MINIMAL repro is `void f(u8 *p) { if (p == NULL) { return; } }`.

So `NULL` (as `types.h` spells it) cannot be compared against a pointer with `==`
in this build. Every unit that tests a pointer argument for null has to use `!p`
or compare with `0`, and a candidate that fails with `<string not found>` should be
checked for a `== NULL` before anything else - it is not a codegen question.

## 2.51 Lift the SDK near-hits with m2c first (2026-11-26)

`tools/match/lift.py` on `arm9 0x02027fe8` (260 B, `PXI_InitFifo`) gave a draft in
one call, and the transcription written from the SDK's own `pxi_fifo.c` plus the
lifted offsets compiled **byte-exact on the first try**. The pattern that works:
lift (or `annotate`) for the offsets and constants, then write the body in the
SDK's statement order, then let the digest decide. The 0.95-0.98 near hits in
`sdkmap.py --near 0.85` are the cheapest bytes on the board and they are cheap
*because* the SDK source already fixes the shape; only the constants differ.

Check `walls.py show <mod> <addr>` first: `func_0202b43c` and several other SDK
near hits are already parked with a register tie, and re-deriving them costs a
pass for nothing.

## 2.52 Which pragmas this compiler actually honours (2026-11-26)

Metrowerks' *C Compilers Reference* documents the optimiser pragmas
(`opt_strength_reduction`, `opt_loop_invariants`, `opt_common_subs`, `opt_cse`,
`opt_dead_assignments`, `opt_unroll_loops`, `peephole`, `schedule`/`scheduling`,
`inline_depth`, `dont_inline`, `pool_strings`), which looked like the answer to
whole wall classes - in particular `opt_strength_reduction off` for the induction
variable in `ov002 0x02080ca4` and `schedule once` for the scheduling parks.
Measured on the 260-byte `func_02027fe8` body, compiled with the project's own
flags and hashed per pragma:

| pragma | .text | verdict |
|---|---|---|
| (none) | 260 B, `8d547c4c` | baseline |
| `optimization_level 0 / 1 / 2` | 272 / 260 / 252 B | honoured |
| `optimization_level 3` | 260 B, same hash | default |
| `optimize_for_size on` | 248 B | honoured |
| `opt_dead_code off` | 248 B (same as optimize_for_size) | honoured |
| `opt_propagation off` | 268 B | honoured |
| `opt_propagation on` | 260 B, same hash | already on |
| `schedule once` / `schedule twice` | 260 B, same hash | **ignored** |
| `scheduling on` / `scheduling off` | 260 B, same hash | **ignored** |
| `opt_strength_reduction off` | 260 B, same hash | **ignored** |
| `opt_loop_invariants off` | 260 B, same hash | **ignored** |
| `opt_common_subs off` | 260 B, same hash | **ignored** |
| `opt_cse off` | 260 B, same hash | **ignored** |
| `opt_dead_assignments off` | 260 B, same hash | **ignored** |
| `opt_unroll_loops off` | 260 B, same hash | **ignored** |
| `peephole off` | 260 B, same hash | **ignored** |
| `inline_depth(0)` | 260 B, same hash | **ignored** |
| `dont_inline on` | 260 B, same hash | **ignored** |
| `pool_strings on` | 260 B, same hash | **ignored** |

So the ARM backend of 1.2/b56 accepts these pragmas and throws them away: the only
codegen levers behind a pragma are the optimisation level, size mode, dead-code
removal and propagation. `pragmas.py` now sweeps just that honoured set by default
(`--all` restores the full list for a future compiler key); a wall whose residue
survives the default sweep is not a pragma problem and the time belongs on the C.

Consequence for two parks: `02080ca4`'s induction variable is *not* switchable off
with a pragma, and the "pure scheduling" parks cannot be re-scheduled with
`schedule once` - both remain source-shape problems.

## 2.53 A mirrored scratch pair: name the intermediate (2026-11-26)

This section was first written as "a mirrored pair is a wall". The next match
disproved that, and the corrected rule is the useful one.

`ov003 0x02066a54` (180 B) reached 180/180 with six slots differing purely as an
`ip`/`lr` swap: b56 put the loop-invariant zero in one scratch register and the
running pointer in the other, retail the reverse. Naming the constant
(`u32 zero = 0`), naming the address (`u8 *q`), reordering the declarations,
casting the stored value, and a 40-variant `respell.py` sweep all produced the
same mirrored pair, which is what made it look like a wall of the allocator.

What actually fixed it: a source that names the intermediate *inside* the loop.

```c
        for (i = 0; i < 0x200; i++) {
            *(u16 *)(0x8de8 + base + i * 2) = 0;
            ip = i * 2 + base;
            if (i % 4 == 0)
                *(u32 *)(ip + 0x8de8) = 0x200;
        }
```

`ip` (a `u32`, assigned after the first store) makes the two stores compute
their addresses independently. That is exactly why retail splits one as
`add #0x8d00` + `[#0xe8]` and the other as `add #0x8000` + `[#0xde8]` instead of
pooling one shared `0x8de8`, and the separate value moves the allocator onto
retail's assignment. The identical body, cloned to `0x020b85b8` and
`0x020db6ec` (same instructions, different pc-relative displacement), matched
both siblings straight away - three units from one shape.

Rule: a mirrored scratch pair is not automatically a wall. Before parking,
name the intermediate address as its own variable inside the loop, even when the
result looks redundant, and check whether retail's two address splits differ -
that difference is the fingerprint of a named intermediate.

## 2.54 Defeating a CSE: inline the base expression at each site (2026-11-26)

`ov003 0x02053b58` was parked because mwcc folded the `st + 0x200` sub-base into one
register where retail recomputes `add r0, r4, #0x200` before each of the four blocks.
Two earlier spellings gave 172 B and 188 B against retail's 236 B.

Inlining the whole base expression at every use - `((u16 *)(st + 0x200))[0x4e]`,
repeated in each `if` and each store - took it to 204 B, the closest so far. The
named pointer (`u16 *p = (u16 *)(st + 0x200)`), the two-pointer form
(`sel` / `dst`), and `volatile u16 *p` all scored worse (188 B, 188 B, 192 B)
because mwcc keeps a named pointer in a register, which is exactly the behaviour
retail does not show.

So when retail recomputes an address that looks loop-invariant, that is evidence
the original source repeated the expression rather than naming it - and for mwcc,
repeating it in the source is how you reproduce the recompute. The remaining
`0x02053b58` gap is elsewhere: retail keeps `(sel - 1)` as the index and 0xbc as the
store offset, while every spelling tried folds 0x5e into the index instead.

## 2.55 A `u16` index is what produces `lsl #16` / `lsr #15` (2026-11-26)

`ov003 0x02053b58` (236 B) closed as soon as the destination index was declared
`u16`, after five spellings stalled at 176-204 B:

```c
    u8 *st = (u8 *)data_ov003_020eb720[0];
    u16 k;
    ...
    if (*(u16 *)(st + 0x29c)) {
        k = *(u16 *)(st + 0x29c) - 1;
        *(u16 *)(st + k * 2 + 0x2bc) = 0;
    }
```

Two things fall out of the `u16`:

- the address is built as `add r0, r4, r0, lsr #15` with a preceding `lsl #0x10`
  (the index zero-extended then doubled), which is the scaling of a 16-bit index,
  not of a `s32` temporary;
- the constant `0x2bc` stays split in the address (`add #0x200` plus a `[#0xbc]`
  store offset) instead of being folded into the index as `0x5d`.

That second half is the same lesson as 2.54 from the other side: a folded constant
in the index (`+ 0x5e` on a `s32`) is the wrong answer whenever retail keeps the byte
offset in the load/store. Declaring the index `u16` prevents the fold.

The function came out byte-identical except for the two known diff artifacts (an
unresolved `bl` printed as a local address and the literal-pool pad word), and the
digest gate accepted it first time.

## 2.56 A pool-register swap is often statement order plus signedness (2026-11-26)

`arm9 0x02027784` (228 B) sat in `walls.txt` as a 2-slot "only the pool registers
differ" park, where retail loads `&data_0204904c` into `r2` and `&data_02049040`
into `r3` and mwcc did the reverse. The fix was two independent changes, and both
were needed:

1. **Statement order at the tail.** Retail's stores go `ldr r1(0x34)`, `str 0x40`,
   `str 0x4c`, `str 0x34`. Writing the three tail statements in that order
   (`data_02049040 = tail` *before* `data_0204904c -= 1`) removed the register
   swap. A pool address is materialised in the order the pool is first referenced,
   so the first statement that mentions each global decides the register.
2. **Signedness.** What remained was `movhi`/`strhi` where retail has
   `movgt`/`strgt`: the wrap counter is compared with `> 8` as a *signed* value.
   Declaring it `s32` instead of `u32` closed the unit.

So when a diff bottoms out at "only the pool registers differ", read which
statement first touches each global and reorder the source accordingly, then check
any conditional in the residue for `hi`/`ls` vs `gt`/`le` - a `hi` where retail
has `gt` means an unsigned local that should be signed.

## 2.57 Three shapes b56 will not reproduce (2026-11-26)

All three were re-attacked with the 2.55/2.56 levers and stayed short of a match.
They are recorded with their best spelling so the next pass starts from the wall,
not from the beginning.

**A 0/1 flag reads eq-first in retail, ne-first in b56.**
`arm9 0x0202a45c` (136 B) ends with `ldr r0,[r5,#0x20]` / `ldr r3,[r5,#8]` /
`cmp r0,#0` / `moveq r0,#0` / `ldr r1,[r5,#4]` / `movne r0,#1` / `mov r2,r4` /
`bl`. The winning skeleton is: load `o+0x20` into a local `t`, then `o+8` into a
local `a3`, then set the flag, then load `o+4` into `a1`, then call
`func(flag, a1, v, a3)`. That gets the load order, the interleaved `ldr r1`
**and** all but one slot - `flag = 0; if (t != 0) flag = 1;` emits
`mov r0,#0` unconditionally. Every other spelling is worse or equal:

| spelling | slots |
|---|---|
| `flag = 0; if (t != 0) flag = 1;` | **1** (`mov` vs `moveq`) |
| `flag = (t != 0) ? 1 : 0;` | 2 (ne before eq) |
| `flag = (t == 0) ? 0 : 1;` | 2 (still ne before eq) |
| `if (t != 0) flag = 1; else flag = 0;` | 2 |
| `if (!t) flag = 0; else flag = 1;` | 2 |
| `flag = 1; if (t == 0) flag = 0;` | 2 |
| 0/1 locals with a ternary | 2 |
| `switch (t != 0) { case 0: ... }` | 7 |
| `flag = !!t;` / `(u32)(t != 0)` | 4 |
| inline args, `!= 0`, `== 0?0:1`, separate `f` local | 4 (per the wall) |

b56 canonicalises the predicated pair to ne-first whenever the condition is
`t != 0`, and to `mov` + `movne` whenever the zero is an unconditional default.
The remaining idea is a source where the compare consumes something other than a
loaded word; nothing tried so far does that.

**A hoisted pool base prefers r1 over ip.**
`arm9 0x0202c430` (24 B) is `while (*(volatile u32 *)0x0204ad00 == 1) {}` - a bare
spin, retail 24/24 B. The earlier "3-slot" candidate was a *fakematch*: it
dropped the reload, so `beq` branched two bytes past the load and the loop could
never see a new value. Spelling the access `volatile` fixes the reload and leaves
2 slots: retail hoists the pool address into `ip` (`ldr ip,[pc]` / `ldr r0,[ip]`,
loop target the load), b56 always picks `r1`. Unchanged by a volatile global, a
`volatile u32 *p` local, `for (;;) if (...) break;`, `do/while`, an assigned `v`,
a `one` local and `(v = load) == 1`. In a leaf function with r0 taken by the
result, b56 never chooses ip for a loop-invariant address.

**The head of the size-sorted queue is not quick wins.**
The five smallest delinked units are not C functions:
`0x02033cb0` is `udf #0xffff; bx lr` (a literal word inside `.text`),
`0x0203b2cc` is `cmp r1,#0; bxeq lr` falling into the next function,
`0x0203aeb8` is `push {r4-r7,fp,ip,lr}; mov r4,#0; b ...` continuing past its 12 B,
`0x02033bc0` is two `mov r0,#n; b <next function>` entries of a table, and
`ov003 0x02050f2c` is `cmp r0,#0; moveq r0,#0; movne r0,#0; bx lr`, which b56
folds to `mov r0,#0; bx lr` (8 B) for `a ? 0 : 0`, `if (a) return 0; return 0;`
and `(a == 0) ? 0 : 0` alike. Sorting the queue by size therefore reaches shared
tails and data first; filter to units whose size covers a whole `push`/`pop` pair.

## 2.59 Breaking the ip/lr scratch tie (2026-11-26)

Six units fell to one lever. The shape is a `%4` reset loop over a fixed offset,
or a block of zero stores into a local struct that is later passed by pointer.
Retail puts the *constant* in `lr` and the pointer in `ip`; b56 swaps them:

    retail                            b56
    mov  lr, r5        ; zero         mov  ip, r5        ; zero
    add  ip, r4, r5, lsl #1  ; base+2i add  lr, r4, r5, lsl #1

The tie breaks on **how the pointer is spelled**, not on declaration order. Two
spellings reproduce retail, and both put the pointer in a `u32` rather than a
pointer-typed local:

1. **Constant-first address plus a named integer the source assigns itself**
   (`ov005 0x020614a4`, `0x020656d8`, `0x020695bc`, `0x0206bfc0`, 4 x 220 B):

       u32 ip;
       u32 base = data_ov005_020c9740[0];     /* integer, not u8 * */
       u16 i;
       for (i = 0; i < 0x200; i++) {
           *(u16 *)(0x4a58 + base + i * 2) = 0;   /* constant first */
           ip = i * 2 + base;
           if (i % 4 == 0)
               *(u32 *)(ip + 0x4a58) = 0x200;
       }

   `base` must be a `u32`; the first store must lead with the constant (that is
   what makes b56 split it as `add #0x4a00` + `[#0x58]`); `ip` must be assigned
   *between* the two stores as its own statement. This is the same source shape
   as the already-matched `ov003 0x020d72f8` / `0x020d9f2c`, so those two are the
   reference implementation.

2. **Take the local's address as an integer, then cast back at the call**
   (`ov009 0x0204f93c`, `0x02051080`, 2 x 212 B):

       u32 ip;
       ...
       ip = (u32)&s2;
       *(u32 *)ip = 0;
       *(u32 *)(ip + 4) = 0;
       *(u32 *)(ip + 8) = 0;
       ...
       func_02022994(&s1, &s3, (Tri *)ip, 1, 0);

   `Tri *z = &s2;` (pointer typed) keeps the address live and b56 sinks the zero
   stores below the next copy, giving 208 B. Declaring the three `Tri`s as one
   `Tri v[3]` array is worse (frame 0x60, 208 B, everything reallocated). The
   `u32` cast keeps b56's address materialisation where retail has it.

Generalisation: when a tie is "constant in lr vs ip", give the pointer an
integer home that the source assigns explicitly, and write the addressed
constant in front of the base. Pointer-typed locals and `&local` spellings let
b56 rematerialise and lose the tie.

## 2.60 `volatile` for retail's repeated loads (2026-11-26)

Retail sometimes reads the same location two or three times where a `C` source
would read it once. b56 always CSEs the repeat, which then changes the whole
live range and the frame. Marking the field `volatile` is the spelling that
reproduces the repeats:

    volatile u8 *cnt = base + 0x36d8;      /* ov002 0x0204f478 / 0x0204f50c */

`ov002 0x0204f478` (148 B) reads the index byte three times (count lookup, slot
base, counter increment); the plain spelling CSEs to one, keeps the pointer in
`r4`, and never matches. With `volatile` all three loads come back and the
function matches exactly. Two further spellings were needed there:

- the increment is `*(u32 *)(p + 0xc10 + *cnt * 4) += 1;` — a compound
  assignment reads the index once; writing `x = x + 1` with the `*cnt`
  expression twice makes the volatile rule emit two index loads (14 diffs).
- the read and the increment split `0xc10` opposite ways: the read is
  `p + *cnt * 4 + 0xc10` (constant as displacement) and the increment is
  `p + 0xc10 + *cnt * 4` (`add r3, ip, #0xc10` first). Constant placement is
  per-site.

Not every repeat is a win: on `arm9 0x02028920` the same double read of
`*(u32 *)(p + 0x28)` needs `volatile`, but that spelling also makes b56 drop the
`push {r4, lr}` frame that retail has, so the pair stays parked. When `volatile`
changes the frame, the repeat is not the real difference — look at which
argument is live across the call instead.

## 2.61 cdiff and arm9's pre-listed delinks (tool)

`cdiff.py` used to append a delinks entry unconditionally. For arm9 every
function is already listed, so the duplicate made configure drop the compile
rule and every probe failed with `no known rule to make it`. It now appends only
when `src/<mod>/<name>.c:` is absent, which fixes all arm9 probing.

## 2.62 Three cheap spellings found sweeping arm9 (2026-11-26)

arm9 0x0201ce4c, 0x020179c8, 0x020202b4 and 0x0202cff8 fell to ordinary source
shapes; the ones worth remembering:

- **Separate base and field pointers** (`0x0202cff8`). `*(u32 *)(data + 0x34)`
  with `data` an array and `+0x34` spelled inline is constant-folded into a
  pool literal, and a `struct` spelling folds the offset into the load
  displacement. Retail has `add r1, r2, #0x34` over a base register that is
  reused for a second field, which needs both a base pointer *and* a second
  pointer made from it:

      u8 *b = (u8 *)data_0204aec0;
      u8 *f = b + 0x34;
      *(u32 *)f &= ~0x20;                 /* add r1, r2, #0x34 ; ldr/str [r1] */
      func_02025118(*(u32 *)(b + 0xd0));  /* ldr r0, [r2, #0xd0] */

- **Two RMWs stay two RMWs** (`0x020179c8`). `field = (field & ~0xff) | v;` is
  one bic+orr with a single store; retail stores the masked word, reloads, then
  stores the OR. Writing `field &= ~0xff; field |= v;` as two statements
  reproduces it, and the address is then CSEd into one register.

- **Early return to stop if-conversion** (`0x020202b4`). With both bodies small,
  b56 predicates the second one too (`movne`/`ldrne`/`bicne`/`strne`) and the
  unit comes out 4 bytes short. Writing the *large* branch first, with an
  explicit `return` after it, leaves the zero case as the only predicated block,
  which is retail's layout. Pool-pointer register order then follows statement
  order: assigning the store target before reading the value gave `r0`/`r1`
  retail's way round.

- **A plain `if` chain beats a predicate** (`0x0202cff8`): two early-return
  guards reproduce `addne sp` / `ldmne sp!` / `bxne lr` exactly.

## 2.63 Switch dispatch: give every case an arm (2026-11-26)

`arm9 0x0201a98c` (196 B) is a callback dispatcher. A `switch` over 0..2 with two
distinct bodies compiles to a compare chain (`cmp r0,#1` / `beq` / `cmp r0,#2`),
while retail has the jump table:

    cmp r0, #3
    addls pc, pc, r0, lsl #2
    b   default
    b   case0          ; table index 0 is the word after `b default`

Writing the switch with all four values 0..3 present — including `case 3:
break;` with no body — makes the range contiguous and b56 emits the table. Read
the table backwards from the `addls`: entry *n* is at `addls + 8 + 4*n`, so the
first `b` after it is index 0, not the last one.

Also note the two live-pointer reloads in that unit: the callback arguments read
the global through one dereference and the dispatch guard through two, and both
reload `data_020475c4` from the pool rather than keeping it in a callee-saved
register — spelling each access out inline is what reproduces that.

## 2.64 Split base from index to move a callee-saved colour (2026-11-26)

`ov003 0x02061cc8`/`0x02061f28` (608 B each) are twins that build two pointers
from one index:

    i = a3 * 0x418;                 ; r8 in retail
    w = (u32 *)(p + 0x3c0 + i);     ; r7 in retail
    b = p + 0x3d4 + i;              ; r6 in retail

Written that way b56 emits exactly retail's instruction stream but colours `i`
into r7 and `w` into r8 — the pair is swapped, 29 relocations and nothing else.
Eight reorderings (declaration order, `s32 i`, `u32 i, *w`, index inlined into
both addresses, a third dummy live local, a separately named base pointer) all
reproduce the same swap; `permute.py` found no state below 29 structural.

What moves it is splitting each `base + index` pointer into two assignments:

    i = a3 * 0x418;
    w = (u32 *)(p + 0x3c0);
    b = p + 0x3d4;
    w = (u32 *)((u8 *)w + i);
    b = b + i;

The emitted code is unchanged — the same `add r1, sb, #0x3c0`, `mul`, `add r0,
sb, #0x3d4`, `add r7, r1, r8`, `add r6, r0, r8` — but the allocator now starts
`i`'s live range first and gives it the higher register, which is retail's
colouring. So when a folded address and a bare index compete for callee-saved
registers, the fix is not the declaration list but which *statement* first makes
the pointer live.

The twin differs only in its table symbol (`data_ov003_020eb0f4` rather than
`data_ov003_020eb124`) and in the two pooled loads; everything else, including
the four unrolled corner blocks reading `q.b, q.a, q.c, q.d` from a struct copy
that b56 emits as `ldm`/`stm`, is shared.

## 2.65 Give a read-modify-write its own pointer variable (2026-11-27)

Two units from the same file family, `ov003 0x020b6d94` (200 B) and
`0x020b7e14` (188 B), turned on the same shape. Retail materialises the address
of an incrementing counter as a pooled constant plus an add:

    ldr   r1, [pc] = 0x0000b636
    add   r2, r4, r1
    ldrh  r1, [r2]
    add   r1, r1, #1
    strh  r1, [r2]

Written as one expression — `*(u16 *)(p + 0xb636) = *(u16 *)(p + 0xb636) + 1;` —
b56 folds it the other way into `add r1, r4, #0xb600` plus `[r1, #0x36]`, four
instructions where retail has five. Giving the address a variable of its own
reproduces retail exactly:

    {
        u16 *v = (u16 *)(p + 0xb636);
        *v = *v + 1;
    }

The same split also fixes the *condition* form: the compare in these units keeps
the folded base (`add r0, r4, #0x8800` + `[r0, #8]`), so the folded spelling
belongs in the `if` and the pointer spelling in the body. A signed counter needs
the pointer typed `s16 *` to get `ldrsh`, while its store stays `strh`.

This undid a wall: `0x020b7e14` had been parked as an unreachable address-
materialisation residue at 24 bytes short, and with the pointer spelling it came
in at its exact size with only relocations left.

## 2.66 A one-bit bitfield for `lsl`/`lsr` flag tests (2026-11-27)

Retail sometimes tests a single bit of a word with a shift pair rather than a
mask:

    ldr   r0, [r6]
    lsl   r1, r0, #0x1f
    lsrs  r1, r1, #0x1f
    beq   ...

Every mask spelling of the test — `(v & 1) != 0`, `((v << 31) >> 31) != 0` —
is simplified by b56 back to `ands r1, r1, #1`, which is one instruction short
and pushes everything after it out of place. A one-bit bitfield gives the shift
pair:

    typedef struct { u32 f0 : 1; } Bits;
    ...
    if (!((Bits *)v)->f0)
        return;

Two adjacent bits (`lsl #0x1e` / `lsr #0x1f`) need two fields, `f0 : 1; f1 : 1;`.
This is how `arm9 0x0200800c` and `0x02014780` got from 40 and 47 differing
slots down to 11 and 44 with the size right; it also fixed the first instruction
of the function, so it is worth checking before any register work.

## 2.67 The four 0x158-entry siblings share one fold wall (2026-11-27)

`ov002 0x020b5088`, `0x020dbfd0`, `0x020ef334` and `0x0209ea50` are four
functions over the same table: `p = *(u8 **)data_ov002_02102070`, an index of
`a0 * 0x158`, a byte at `p + idx + 0x2e6`, another at `p + idx + 0x2e4 + 2`, a
`data_ov002_020fb544 + f*0x34` row, and two calls into `0x02057c4c` /
`0x02057a9c`. All four compile to their exact retail size, and all four carry
the same residue: retail keeps *two* bases alive — `p + 0x2e4` used as an
indexed load `[base, idx]` and `p + idx` used with constant offsets — while b56
folds both reads onto whichever one it picked.

Spellings tried on the family: array subscript vs pointer arithmetic, the two
reads in either order, a separate `q = p + idx`, `e += idx` after the reads, and
declaration-order permutations (the permuter found no better order). In
`0x0209ea50` retail even materialises `e + idx` explicitly, so the split is not
"indexed always wins" — it follows how many constant-offset users each base has.

The offsets themselves are one more small wall: retail builds the `0xa0000`
stride offset as `rsb ip, r1, #0` unconditionally and then multiplies it by `-1`
in the equal branch (`mvn r1, #0` + `mul`), where every ternary spelling of the
same thing compiles to `mov` + `rsb` inside the branch.

Leave these parked — the fold decision is not reachable from C spelling here.

## 2.68 Break a repeated subexpression to stop mwcc holding a pointer (2026-11-27)

When the same `base + offset` appears in several statements, b56 folds it into
one register and keeps the *base* alive in another. Retail usually recomputes
it at each use, which costs one instruction more per site but frees a
callee-saved register — and the freed register is what the function's frame
shape depends on.

`ov002 0x020870fc` would not come in at its 196 bytes: retail pushes `{r4, lr}`
and recomputes `q + 0x1ec` three times, while every straightforward spelling
pushed `{r4, r5, lr}` with a 4-byte spill, because mwcc held `q + 0x1ec` in r4
and `q` in r5. Spelling the three uses slightly differently made mwcc treat them
as three separate trees:

    func_0203447c((u32)((u8 *)q + 0x1ec), 0, 0x200);
    func_02034494(q + 0x1ec, (u8 *)((u32)q + 0x18), 0x1d4);
    switch (func_02005958(3u, (u32)q + 0x1ec)) {

That is the match. The same trick fixed the tail of `ov003 0x0209a354`
(`(u32)((u8 *)p + 0x8de8)` then `(u32)p + 0x8de8`).

The tell is a frame that is one register too large or one stack slot too big
with no other structural difference: look for a subexpression used two or three
times and spell each occurrence differently.

### 2.69 A redundant re-test means a nested `if`, not `else if`

`ov003 0x02051b30` has a two-arm dispatch on a state word, and retail tests the
state *twice* in the non-zero arm:

```
cmp r0, #0
beq end
cmp r0, #1
bne skip
```

The second `cmp r0, #0` is dead by construction, so it is not something the
compiler adds — it is in the source. Written as `if (st == 0) {...} else
{ ... }` mwcc emits one test and lands 8 bytes short. Written as two sibling
`if`s it emits both tests but forces the pointer to stay live across the arm
that contains calls, which costs a callee-saved register and 4 bytes of frame.

The spelling that matches is the nested one:

```c
if (st == 0) {
    ...arm with calls...
} else {
    if (st != 0) {          /* "redundant" by hand, and retail has it */
        if (st == 1)
            clear();
        st--;
    }
}
```

Rule: a dead compare in retail is written by the original author, not produced
by mwcc. Keep the `if/else` join and put the redundant test *inside* the else.

### 2.70 Clearing a local vector through a pointer fixes the spill order

`ov003 0x0205e228` builds three 12-byte vectors on the stack. Assigning the
three fields directly makes mwcc store with `str r0, [sp, #0x44]`-style
addressing and hoist the later data load above the clear, which reorders the
whole tail. Clearing through a pointer to the same object keeps the base
address in a register and stores through it, and that reproduces retail exactly:

```c
{
    s32 *tp = (s32 *)&target;

    tp[0] = 0;
    tp[1] = 0;
    tp[2] = 0;
}
```

Two companions from the same function, both worth trying first on stack-heavy
units: write a mask as `& ~0x3000` (gives `bic r2, r2, #0x3000`) rather than
`& 0xcfff` (gives a pooled `and`); and give a `u16` + `u8[]` pair an explicit
`u16` pad field to reproduce a frame slot at `+4` with the array at `+8`.

### 2.71 A byte counter read twice needs two spellings

`ov002 0x02081cfc` ends with a saturating 0..11 frame counter. Retail addresses
it through a computed base for the increment and sp/field-relative for the
compare:

```
add r1, r4, #8
ldrb r0, [r1]
add r0, r0, #1
strb r0, [r1]
ldrb r0, [r4, #8]     <- re-addressed, not [r1]
cmp r0, #0xc
```

Writing the increment and the wrap test the same way gives one CSE'd base and
lands 4 bytes short. Taking the address for the increment only, and coding the
test as `a[8]`, reproduces it:

```c
{
    u8 *cnt = a + 8;

    *cnt = *cnt + 1;
}
if (a[8] >= 0xc)
    a[8] = 0;
```

### 2.72 Keep the byte offset in its own variable when it is reused

`ov002 0x020b5b74` indexes a 0x158-byte array twice, once as `p + 0x2e4 + n*0x158`
and once as `p + n*0x158 + 0x2e6`. Folding the multiply into each address lets
mwcc emit `mla` for the first and an `add` for the second, and the function
lands 4 bytes short with a different register layout:

```
mla  r4, r0, r2, r4       <- mine
add  r3, r0, r3
```

Retail keeps the scaled offset as its own value and multiplies once:

```
mov  r1, #0x158
mul  r3, r0, r1
add  r2, r1, #0x2e4
add  r5, r2, r3           <- e = p + 0x2e4 + off
add  r3, r1, r3           <- p + off
ldrb r0, [r3, #0x2e6]     <- m
```

So write:

```c
s32 off = a0 * 0x158;
u8 *e = p + 0x2e4 + off;

m = *(u8 *)(p + off + 0x2e6);
```

The tell is an `mla` in your output where retail has a bare `mul`: retail's
source named the scaled index as a variable, so the compiler had no reason to
fuse the add.

### 2.73 `x * (c ? -1 : 1)` emits `mvn` + `mul`, `x * -1` emits `rsb`

A constant negation is peepholed away, which can hide the original source and
cost instructions. `ov002 0x0205a670` picks a horizontal kick direction:

```
cmp  r2, #0                ; h == 0 ?
mov  ip, #0xa0000
mvneq r1, #0               ; -1
muleq r1, ip, r1           ; 0xa0000 * -1
moveq ip, r1
```

Writing `dx = 0xa0000 * -1` or `dx = -0xa0000` gives a single `rsbeq r0, r0, #0`
and the unit lands 8 bytes short. The form that matches is a ternary whose other
arm is the value itself:

```c
dx = 0xa0000 * (h ? 1 : -1);
```

Because `0xa0000 * 1` needs no work, the `h != 0` arm costs nothing, and the
`h == 0` arm materialises `-1` in a register for a real `mul`. Whenever retail
has `mvn` feeding a `mul`, look for a multiply whose second operand is chosen by
a condition rather than being a literal.

### 2.74 The UPDATE families call `func_ov002_02057068` with twelve arguments

Every 0x158/0x1744/0x1824 UPDATE family member (756 B, 688 B, 676 B, 608 B)
ends in the same call, and it takes **twelve** arguments, not the eleven the
m2c lift suggests. Retail stores eight words to the outgoing area:

```
mov  r3, #0
str  r3, [sp]              ; a4  = 0
str  r0, [sp, #4]          ; a5  = -0x4074 (or the member's constant)
str  r0, [sp, #8]          ; a6  = half
str  r0, [sp, #0xc]        ; a7  = 0x42000000
str  r0, [sp, #0x10]       ; a8  = 9
str  r1, [sp, #0x14]       ; a9  = *e
str  r0, [sp, #0x18]       ; a10 = e->u16
str  r0, [sp, #0x1c]       ; a11 = a0
```

Four register arguments plus eight stack words means `a3` and `a4` are both
zero. Writing the call with one zero instead of two leaves the unit one
instruction short (604 of 608 on `0x0208f38c`, 740 of 756 on `0x0208f860`),
and adding it makes both size-exact. The extern must be declared with twelve
parameters or mwcc drops the argument.

Two companions from the same function, both already in the table but worth
repeating here because they stack: `s16 ang = 0; ang = ang + f();` produces the
`add r0, r0, #0` that retail has after `func_ov002_02053468`, and
`u8 half = (u32)b >> 1` is the `lsr` retail uses where plain `b >> 1` gives an
`asr`.

### 2.75 A masked call argument needs a `u8` parameter AND a wider local

`ov002 0x020a82c4` passes a mode byte chosen between 0x2d and 0x2e, and retail
masks it right before the call:

```
ldrb r1, [r4, #1]
mov  r0, r2
and  r2, r3, #0xff
bl   0x204d270
```

Writing `u8 flag` as the local and `u32` as the parameter gives no mask at all
(192 of 200 B): mwcc knows both constants fit, so the truncation is dead.
The pair that emits it is a **wider local with a `u8` parameter**:

```c
extern void func_ov002_0204d270(u32 a0, u32 a1, u8 a2);
...
s32 flag;           /* the local must be wider than the parameter */
```

That is worth 4 bytes here. When retail truncates an argument before a call,
check the parameter width first — if it is already narrow the local has to be
wider, and a `u8` local silently removes the instruction.

### 2.76 `var3` cannot catch a wrong data symbol, only the link can

`ov002 0x0205a060` reported `MATCH` from `var3` with `extern u8
data_020fb544[]`, and `reg.py` refused it: the build died with

```
mwldarm.exe: Undefined : "data_020fb544"
```

The symbol is `data_ov002_020fb544` — overlay symbols carry the `_ov00N_`
infix. `var3` masks relocation bytes, so it cannot tell a resolvable symbol from
an undefined one; a name that does not exist in `symbols.txt` still compares
byte-identical. `annotate.py` prints the right name next to each pooled literal
(`= 0x020fb544  data_ov002_020fb544 (data)`), so copy it from there rather than
guessing between the `data_<addr>` and `data_<mod>_<addr>` forms.

The same trap applies to a spelled-out literal: `*(u32 *)0x020fb544` links to
the right bytes and passes both `var3` and the digest, but scores below 100% in
objdiff. Use the symbol name from `annotate.py`.

### 2.77 Two levers for the `0x158`-stride entity setters

`ov002 0x02092988` and its neighbours are built from the same ingredients, and
two spellings decide whether the instructions appear at all:

- The byte fetched at `p + idx*0x158 + 0x2e6` must be a `u8` local, not `u32`.
  `ldrb` zero-extends, so `u8 m = *(u8 *)(p + off + 0x2e6);` feeds
  `mov r0, m` with no extra `and`; a `u32` local makes mwcc re-load or mask.
- A `+/-` constant pair chosen by a compare wants the if/else *assignment* form
  `if (c) v = -A; else v = A;`, which gives retail's
  `movlo r2, #0x100000 / rsblo r3, r2, #0 / movhs r3, #0x100000`.  Building it
  as `v = (c ? -1 : 1) * 0x100000` still folds, but putting the pair inline in
  the arithmetic hides it from the scheduler.

`ov002 0x02088b0c` (matched) also confirmed §2.71 from the other direction: the
guard reads the counter with the field form `*(u8 *)(a + 0x37a)` and the body
re-reads it through a named pointer (`u8 *c = a + 0x37a; *c = *c + 1;`), which is
what produces retail's pool-loaded `0x37a` and the five-instruction RMW.  The
same statement written as one `*(u8*)(a+0x37a) = *(u8*)(a+0x37a) + 1` is three
instructions and 16 B short.  Its tail store must be an if/else on the store
only -- `if (...) *(s16*)(r+6) = 0xf3; else *(s16*)(r+6) = 0xb3;` -- which
if-converts to `moveq/strheq + movne/strhne`; a ternary collapses to one `strh`,
and an early `return` turns both into branches.

### 2.78 Two bases 0x300 apart are one base with two offsets

`ov002 0x020894f0` (matched) mutates what `lift.py` reports as
`*(s16 *)(p + 0x4ec4 + 0x66)` and `*(s32 *)(p + 0x4bc4 + 0x364)`. Written as two
pointers (`u8 *w = p + 0x4bc4; u8 *t = p + 0x4ec4;`) the function compiles to
228 B, 12 B over; retail's `add r0, r4, #0x300 / ldrsh r2, [r0, #0x66]` proves it
uses one base and the raw offset `0x4bc4 + 0x66 = 0x4c2a`... which is 0x366 from
`p + 0x4bc4`. So when two of a draft's bases differ by a multiple of 0x100 and
retail materialises the larger one as `add rX, <small base>, #0x300`, collapse
them into the small base and add the difference to the field offsets. Rewriting
`*(s16 *)(t + 0x66)` as `*(s16 *)(w + 0x366)` was the whole fix.

### 2.79 Re-materialise a pool-derived pointer per call site

`ov002 0x02089210` (matched) loads four pointers out of the module struct
(`p + 0x4fec`, `0x4ff0`, `0x4ff4`, `0x4fe8`) and makes two 8-argument calls that
differ only in a constant and in one index.  Hoisting the four pointers into
locals is the natural way to write it and comes out **208 B against retail's
236**: mwcc keeps them in r4-r8 across the call and pushes five registers.
Retail instead re-loads `data_ov002_02102070` *and* each field for the second
call, so the source spelled the whole expression twice.  Writing the four field
loads inline in both argument lists — repetitive to read, one word shorter per
use in the object — reproduces retail exactly.

The rule: when retail's second call in a pair contains a fresh `ldr rX, [pc]` of
the module pointer, the original source did not hoist anything, and a testable
signal is that a hoisted spelling comes out *under* the retail size because the
callee-saved push count drops.

`ov002 0x020857ec` and `ov002 0x020b9c38` are the same lesson in reverse: both
need their `0x38`-scaled bases and their `0x158`-scaled entity offsets spelled
per use, and their remaining gap is colouring only (see walls-detail).

### 2.80 `include/types.h` does not define `NULL`

`types.h` is 20 lines of typedefs.  A candidate that writes `if (p != NULL)`
compiles to **nothing** at all — `var3` prints `got 0 want N` with no
diagnostic, because the harness only surfaces the missing object, not mwcc's
"undefined identifier".  Use a plain `0` for pointers.  `arm9 0x0201f2b0`
matched on the first try once the four `NULL`s became `0`.

When `var3` reports `got 0`, suspect a compile error rather than a shape
problem, and get the diagnostic by compiling the body by hand with the harness
flags (`./tools/sjiswrap.exe tools/mwccarm/1.2/b56/mwccarm.exe -c <file> -o
<obj>` plus the include path), not by guessing at the C.

### 2.81 `volatile` on a local forces the store-then-reload pair

Retail sometimes contains `str r3, [sp, #8]` immediately followed by
`ldr r0, [sp, #8]` — a store dictating a live local that then feeds an argument,
and a frame 8 bytes larger than the call's own stack arguments need.  A plain
`s32 z = 0;` folds to `mov r0, #0` and the unit comes out 8 B short
(`arm9 0x0202ccec`: 212 vs 220).  Declaring the local `volatile s32 z;` and
assigning `z = 0;` reproduces the pair exactly.  The cost is ordering: mwcc
hoists the volatile store above neighbouring non-volatile stores, so this only
closes a unit whose retail listing has the spill at the top of the block.

### 2.82 `x += 1` on a struct field is a *second* load, `x = x + 1` is not

A digit/fill loop that reads a counter and then bumps it usually wants the
retail pattern `ldrb r1, [r5]` / `ldrb r7, [sl, #4]` / `add ip, r1, #1` /
`strb ip, [r5]` — two loads of the same byte, then one store.
`i = s->count; s->count = s->count + 1;` CSEs to a single load.  Writing the
bump as `s->count += 1;` makes b56 read the byte again for the compound
assignment, which is exactly retail's pair (`arm9 0x02037590`).

The same unit's tail needed a separate temporary for the end pointer: with
`q = p + s->count - 1;` mwcc folds `p = s->digits` (`s+5`) and the `- 1` into
`(s+4) + count`, giving `add r1, sl, #4 / add r2, r1, r0`.  Retail keeps
`add r0, r3, r0` then `sub r2, r0, #1`, which you get by spelling the
intermediate: `e = p + s->count; q = e - 1;`.

### 2.83 Declare the register that must survive a call *last*

Retail's callee-saved registers are handed out in declaration order, so the
order of the locals decides the prologue.  `ov002 0x02076a58` was
instruction-for-instruction identical except that the four parameters came out
in `sb, r8, r7, r6` instead of retail's `r8, r7, r6, r5`.  Everything above,
including the whole body, already matched; declaring the one local that is live
across the `bl` (`u8 *sb;`) *after* the others moved the parameters down a slot
and the unit matched with no other change.

Generalisation: when a 200-odd-byte unit has the exact right instruction
inventory but the whole listing is shifted by a fixed register rotation, permute
the declaration order before changing any expression.

### 2.84 A switch whose every case is empty still gets its jump table

`ov003 0x02066f48` dispatches on `sel < 10` with `addls pc, pc, sel, lsl #2` and
ten words that all branch to the epilogue except one.  `switch (sel) { case 7:
…body…; }` collapses to a single compare (176 B vs 216 B); the table only
appears when every case 0..9 is written out, grouped, with `break;`.

Watch the case number: because `add pc, pc, sel, lsl #2` reads PC as
`insn + 8`, the first table word is dead and `sel == 0` lands on the *second*
word.  The body therefore sits at whichever case number is one less than the
naive `(target - table_start) / 4`.  Retail's body was at table word 7 but at
`case 6:`.

### 2.85 A missing `mov r0, <local>` means the source passes the previous call's result

`ov005 0x0204c914` fills a stack struct, calls `func_ov005_0204f1c0(0, 0xc0, i, &b)`,
stores the result, and then calls `func_ov005_0204f2ac(...)`.  Writing
`func_ov005_0204f2ac((u8 *)&b, 0xd, 0, 0x320)` puts an extra `mov r0, fp` in
front of the second call (216 B against retail's 212).  Retail has no such
instruction: r0 still holds the first call's return value, so the original
source passes that value, not `&b`:

```c
v = func_ov005_0204f1c0(0, 0xc0, i, &b);
*(u32 *)(e + 0x5c68) = v;
...
func_ov005_0204f2ac((u8 *)v, 0xd, 0, 0x320);
```

Generalisation: before adding a cast or a temporary to make an argument
"obvious", count the moves retail spends on each argument.  A missing move is
evidence about what the original expression was.

### 2.86 `ldrb` + `blt`/`ble` means a signed counter, not a `u8` comparison

The same unit's loops reload a byte (`ldrb r2, [r0]`) and then branch with
`blt`, and the counters step with `add rX, rX, #1` right before `cmp`/`blo` on
`u16` counters but `blt` on the byte bounds.  Spelling the counters `s32 i,
j;` turned b56's `blo` into retail's `blt`; `u32` gave `blo`, and an `s8`
pointer gave `ldrsb` where retail has a plain `ldrb`.  So the mix is: `u8 *`
for the byte data, signed counters for the comparisons.

Two more shapes from the same batch worth knowing:

- `e->buf[j] = s[j + 1];` with a `u8 *` source produced
  `add r4, r1, #1 / ldrb r3, [r0, r4]`; writing `u8 *q = s + j; e->buf[j] =
  q[1];` produced retail's `add r2, r0, r1 / ldrb r3, [r2, #1]`.  When retail
  builds a base pointer and then uses a small offset, give the base its own
  local.
- `arm9 0x02015e64` needed the *opposite* of §2.81: a plain `u32 z; z = 0;`
  inside the loop is what creates retail's `str r5, [sp]` / `ldr r0, [sp]`
  pair, and `volatile u32 z;` reproduces it when b56 keeps it in a register.

### 2.87 A cast on the second reference stops a pooled symbol being cached

`ov009 0x020500d8` passes the same array address to two calls in a row:

```c
func_02025c94((u32)data_ov009_02052adc, 0x200);
func_02023e58((u32)data_ov009_02052adc, 0xbb80, 0x200);
```

b56 kept the address in callee-saved `r5` and emitted `mov r0, r5` at both call
sites (224 B).  Retail loads it from the pool twice (`ldr r0, [pc, #0x64]` …
`ldr r0, [pc, #0x58]`), so the value never spanned the first call.  Spelling the
second reference through a different type does it:

```c
func_02023e58((u32)(void *)data_ov009_02052adc, 0xbb80, 0x200);
```

The two expressions no longer CSE, so b56 rematerialises the address at each
site and the frame drops to retail's 220 B.  Related to the address-
materialisation note: when retail reloads a pooled word that b56 hoists, make
the two references not textually identical (a `(void *)` or `(u8 *)` cast is
enough) before reaching for `volatile`.

### 2.88 A chain of tail-call arms ends with an epilogue and no `return`

`ov003 0x020640ec` is five arms of `return func_02021d98(a0, 0x22, tbl[a1 + k]);`
for `k = 0, 4, 8, 12, 16`, and retail ends it with

```
cmp   r2, #4
addne sp, sp, #4
ldmne {..}
bxne  lr
```

— an inverted epilogue, i.e. the last arm's exit merged with the fall-off-end
exit, and no `mov r0, #0` anywhere.  That only happens when the source has no
final `return`: write the five `if (sel == k) return …;` statements and stop.
A `return 0;` on the end adds the `mov` and breaks the match.

The arms are also emitted as a `cmp`/`bne` chain rather than a jump table, so a
`switch` is not what the original had (mwcc would density-test it); plain `if`s
in ascending order reproduce it.

### 2.89 One magic, five divisors: the shift tells you which

`0x88888889` is mwcc's magic for the whole `15 / 30 / 60 / 120 / 240` family.
The only thing that differs is the shift after `add ip, x, ip`:

| divisor | 15 | 30 | 60 | 120 | 240 |
|---|---|---|---|---|---|
| `asr`   | 3  | 4  | 5  | 6   | 7   |

`ov002 0x020e66cc` compares a field against `0x12c` right above the division,
which invites `/ 300` — and `/ 300` even compiles to a similar `smull`, but
with a *plain* `asr #5` and no `add` back.  Reading the shift (`#4`) instead
gave `/ 30`.  When a division's magic is shared by a family, compile the
members and pick by the shift, not by the neighbouring constants.

### 2.90 `t *= -1` keeps the multiply, `t = t * -1` folds to `rsb`

Same unit: retail materialises the constant (`mvn r2, #0`) and emits
`mul r2, r1, r2` for a negation inside a one-arm `if`.  `t = t * -1;` folds
that to a single `rsbgt`.  Writing the compound assignment — or a literal
`t = t * 0xffffffff;` — keeps retail's three instructions.  A `mul` operand of
`-1` therefore means the source used `*=` or an unsigned literal, not a plain
negation.

### 2.91 To get an arm if-converted, negate the condition

The clamp above is `if (f4 >= 0 && f10 <= 0) f4 = -0x10000; else f4 = 0x10000;`
and retail predicated the *first* arm (`movle/rsble/strle`) with the second
arm as the fall-through.  Writing the semantically identical
`if (f4 >= 0 && f10 > 0) f4 = 0x10000; else f4 = -0x10000;` gave the mirror
image (224 B, predicated `movgt`).  b56 if-converts the `then` of the condition
as written, so when retail's predicated block is the `else`, invert the
condition instead of rearranging the bodies.

### 2.92 Declaring the scalar before the pointer flips the r4/r5 colouring

Three overlay-002 entity units (`0x02092988`, `0x020d1234`, `0x020d2e00`) are the
same shape: a base pointer, an entity pointer `base + 0x2e4 + id*0x158`, and one
byte `b` read from `base + id*0x158 + 0x2e6`.  With

```c
u8 *ent = base + 0x2e4 + id * 0x158;
u8 *g   = base + 0x1824;
u8  b   = *(u8 *)(base + id * 0x158 + 0x2e6);
```

b56 puts the entity pointer in `r5` and the byte in `r4`, while retail has the
entity in `r4` and the byte in `r5` (in one unit the base pointer itself takes
`r5` and the byte reuses it).  The swap looks like an unbreakable colouring tie
and was parked as one.  It is not: declaring the byte **first**, before the two
pointers, makes b56 reproduce retail exactly —

```c
u8 *base = data_ov002_02102070;
u8  b    = *(u8 *)(base + id * 0x158 + 0x2e6);
u8 *ent  = base + 0x2e4 + id * 0x158;
u8 *g    = base + 0x1824;
```

`0x02092988` matched on that change alone; `0x020d1234` had the same swap and
matched once §2.93 was applied too.  So before parking a swap of exactly two
saved registers, try moving the *scalar* (non-pointer) definition above the
pointer definitions.

### 2.93 Compute a conditional value before the pair of stores that use it

`ov002 0x020d1234` stores `g->fac` into two entity halves with a conditional
`off` computed between them:

```c
*(u16 *)(ent + 0x8e) = *(u16 *)(g + 0xac);
off = (b < 2) ? 0x160000 : -0x160000;
*(u16 *)(ent + 0x8c) = *(u16 *)(g + 0xac);
```

That compiles to the same size but takes `r0` for the first load where retail
uses `r1`, and the diff is a single register name.  Hoisting the `off`
assignment above both stores fixes it:

```c
off = (b < 2) ? 0x160000 : -0x160000;
*(u16 *)(ent + 0x8e) = *(u16 *)(g + 0xac);
*(u16 *)(ent + 0x8c) = *(u16 *)(g + 0xac);
```

Moving the *last* statement below the stores does not (`f` variant, still one
register off).  When a single load temporary is coloured `r0` where retail has
`r1`, look for a value that retail computed before it, not after.

### 2.94 `u8 x = v / 2` is `lsr` plus the truncation mask; `>> 1` is not

`ov002 0x020b357c` divides a byte field by two.  Retail emits

```
ldrb r0, [r5, #2]
lsr  r0, r0, #1        ; logical, not asr
cmp  r0, r6            ; ...
and  r6, r0, #0xff     ; on the other one
```

`u8 tgt = *(u8 *)p >> 1;` gives `asr` (the byte promotes to signed `int`), and
once the operand is widened to `u32` to force `lsr`, mwcc drops the `and`
because it can prove the result fits in a byte.  Writing the division instead —
`u8 tgt = *(u8 *)p / 2;` — produces both: `lsr` (unsigned division by a power of
two) *and* the `and #0xff` truncation, because the quotient's source type is
`int`.  So when retail has `lsr` **and** a byte mask on the same value, the
source divided, it did not shift.

A reminder on stack struct reads: with the two `Vec3` copies at `sp+0` and
`sp+0xc`, retail's `ldr r0, [sp, #0x10]` is `c[1].y`, not `c[1].x` — index by
the 12-byte stride before naming the field.

### 2.95 Conditional decrement, unconditional store

`ov003 0x020c7c38` ends with

```
cmp  r4, #0
subne r0, r4, #1
lslne r0, r0, #0x10
lsrne r4, r0, #0x10
strh r4, [r6]          ; unconditional
```

`if (n != 0) *p = n - 1;` gives a *predicated* `strhne` and no truncation, which
is 8 bytes short.  Retail updates the `u16` variable and then stores it — so the
source is

```c
if (n != 0)
    n = n - 1;
*p = n;
```

The unconditional store is the tell: when retail stores a value outside the
`if` but decrements inside it, the decrement was an assignment to the variable,
not a modification of the stored expression.  The `lsl/lsr` pair is that `u16`
assignment's truncation, and it only appears when the variable survives the
store.

### 2.96 Keep the source's own split of a big offset

Three shapes in the same unit:

| retail | source |
|---|---|
| `add r0, g, #0x100` … `ldrh [r0, #0xa8]` | `g + 0x100 + 0xa8` |
| `add r0, g+idx*2` … `add r0, #0x100` … `ldrh [r0, #0x1e]` | `g + 0x100 + idx*2 + 0x1e` |
| `ldrh [sp]` every iteration | loop bound `buf[0]` read from memory, not a cached local |

`g + 0x11e + idx*2` folds to one `add` plus a `0x11e` load offset (12 bytes
short overall).  When retail spends a separate `add #0x100`, the `0x100` was its
own term in the source expression.  Likewise, a struct whose address escapes
into a call is re-read from memory in the loop bound — do not cache the count in
a local.

### 2.97 Struct-by-value parameters: the `push {r0..r3}` + `ldm/stm` pair

`ov002 0x02057f48` opens with

```
push {r0, r1, r2, r3}      ; homes the register arguments
push {r4, r5, r6, r7, lr}
...
ldm  r0, {r0, r1, r2}      ; from sp+0x1c = the homed arguments
stm  r3, {r0, r1, r2}
```

A function whose first instructions spill its own arguments to the stack and
then copies them out three words at a time is not taking seven scalars - it is
taking **structs by value**.  The signature here is

```c
void f(u32 id, Vec3 pos, Vec3 rot, u32 a7);
```

with `Vec3` three `s32`s: `pos` arrives in r1-r3 and is homed at `sp+0x1c`, and
`rot` plus `a7` are already on the incoming stack at `sp+0x28`/`sp+0x34`.  Then
`*(Vec3 *)(ent + 0x90) = pos;` compiles to exactly that `ldm`/`stm` pair, and a
write back to `sp+0x28` is an assignment to `rot.x`.  Seven scalar parameters
never produce the homing push; if you see it, look for a by-value struct.

Two smaller levers from the same unit:

```c
rot.x = rot.x + ((rot.x > 0) ? -0x140000 : 0x140000);
```

gives retail's `movgt r0,#0x140000 / rsbgt r2,r0,#0` delta then one shared
`add`; the `if (x > 0) x -= c; else x += c;` spelling predicated the whole
add/sub twice and came out two instructions short.

And the two difference temporaries must be *computed* in the opposite order to
the one they are declared in - `dx = ...; dy = ...;` with `dy` passed to the
first call was the last four bytes: computing `dy` first gave `sub r6, r3, r0`
with rot.y in r3, retail keeps rot.y in r1.

### 2.98 A 12-byte aggregate assignment is retail's byte-pair copy loop

`ov002 0x020854dc` copies twelve bytes with

```
mov  r3, #6
ldrb r2, [r0], #1
ldrb r1, [r0], #1
subs r3, r3, #1
strb r2, [ip], #1
strb r1, [ip], #1
bne  ...
```

That is not `memcpy` (b56 emits a real `bl memcpy` for `memcpy(d, s, 12)`, and a
hand-written `for (j = 0; j < 12; j++) *d++ = *p++;` gives a 12-trip single-byte
loop).  It is an **aggregate assignment of a 12-byte struct**:

```c
typedef struct { u8 b[12]; } B12;
*(B12 *)(o + 0x156) = *(B12 *)(q + 0x186);
```

which mwcc expands into a word-pair-at-a-time byte copy with a trip count of
`size / 2`.  If a unit copies a small, oddly-sized block and no `memcpy` call is
visible in retail, look for a struct type of exactly that size.

### 2.99 Register colouring for a loop's callee-saved set follows declaration order in reverse

The same unit matched only once the declarations were ordered to suit the
allocator.  b56 hands out `r4, r5, r6, r7, r8, sb, sl, fp` to the live values of
the loop in *reverse declaration order* (the last declared longest-lived pointer
gets `r4`), so retail's

```
r4 = o    r5 = s    r6 = q    r7 = flag   r8 = i    sb = e   sl = ent   fp = dst
```

needs the declarations written as

```c
u8 *ent = ...;   /* sl */      u8 *e = ...;    /* sb */
s32 i;                         u32 flag = 0;
u8 *q = ...;                   u8 *s = ...;
u8 *o = ...;                   u8 *dst = ...;
```

(earlier pointer declarations still get `sl`/`fp`, the 0xc/0x158 cursors land in
`r4`-`r8`).  The instruction stream was byte-identical modulo register names for
three tries before this ordering closed it, so when a diff shows only register
substitutions, permute declarations instead of respelling the code.

Loop-tail statement order is emitted verbatim: retail's `add r6,#0xc` (q),
`add r5,#0xc` (s), `add sb,#0x158` (e) needs `q += 0xc; s += 0xc; e += 0x158;`
in that order, not the reading order of the arrays.

### 2.100 Struct-array indexing keeps retail's scaled addressing

`*(s32 *)(dst + i * 0x20 + 0x1c)` makes b56 strength-reduce the address into a
running byte offset (`lsl r8, i, #5` once, `add r8, r8, #0x20` at the tail) and
keep the cursor in a callee-saved register.  Retail used the scaled-index form
(`ldr r1, [r5, r8, lsl #5]`, re-materialised per statement) and spent the register
on the table pointer instead.  Writing the same bytes as a struct array closes it:

```c
typedef struct { s32 w0, w1, w2, w3, w4, w5, w6, w7; } Dst;   /* 0x20 */
typedef struct { s32 w0, w1, w2, w3, w4; } Src;               /* 0x14 */

dst[i].w7 = dst[i].w2 - src->w3 + tbl[2] / 2;
dst[i].w4 = func_02034148(dst[i].w0 - src->w1);
...
src++;
i++;
```

`dst[i]` forces the scaled index and no live cursor; `dst + i * 0x20` does not.
ov002 `0x0205a260` went from 240 B / 7 pushed registers to a byte match.

### 2.101 `* 0x1111` must be written as the shift-add sum

`v * 0x1111` compiles to `mul r0, r1, r0` with `0x1111` in the pool; retail
expanded it (`lsl #0xc`, `add ... lsl #8`, `add ... lsl #4`, `add`) and stores the
halfword from the sums.  Spell the sum out:

```c
*(u16 *)(g + 4) = (v << 12) + (v << 8) + (v << 4) + v;
```

Same value, 12 bytes shorter, byte-identical for ov002 `0x02062e80`.  Check any
"nice" constant multiply against retail before trusting `mul`.

### 2.102 A value retail re-reads is not a local

ov002 `0x0206a030` reads its table index twice (`ldr r0,[r5,#4]` in the prologue
and again after four calls).  Declaring `u32 i = *(u32 *)(g + 4);` makes b56 keep
it live and the second read disappears; writing the field reference out at each
use reproduces both reads, and the extra `sub sp, sp, #4` that comes with the
re-read shape.  Same rule as 2.99: the instruction stream shows which values the
original source kept in variables.

Also: `(u16)x` truncates with `lsl #16` / `lsr #16` (or a fold), while `x & 0xffff`
gives `and rX, rX, #0xffff` with the mask pooled.  retail had `and r1, r1, r0,
asr #8`, so that halfword store's value is spelled as a mask, not a cast.

### 2.103 Sum operands are evaluated right to left

`if (dy * dy + dx * dx < 0x100)` computes `dx * dx` first in b56; retail's
`sub r1, r7, #0x92` (the y term) came first, so the source has the y term on the
right: `if (dx * dx + dy * dy < 0x100)`.  With `dy`/`dx` as locals the whole
sequence then closes byte for byte (ov008 `0x0204df5c`).  When two symmetric terms
land in the "wrong" order, swap them in the expression instead of permuting
declarations.

### 2.104 Declaration order fixes two-variable colouring swaps

ov002 `0x02092a5c` has three pointers where the short-lived base and the later
byte value share one register (their live ranges do not overlap): retail keeps
them in r5 and the long-lived record pointer in r4.  b56 produced the mirror
image no matter how the expressions were spelled.  The lever is declaration
order: `base`, `off`, `g`, `t`, `e`, `r` closed it, while `base`, `off`, `e`,
`g`, `t`, `r` gave the exact swap.  When a diff is a pure two-register
transposition across the whole function, permute the declaration list before
touching the expressions.

### 2.105 `(u8)` on a wide variable is what asks for `and #0xff`

ov002 `0x020d149c` passes `0x2f`/`0x2e` as a mode argument.  With `u8 mode;`
b56 knows both constants fit and passes them unmasked; retail has
`and r2, r4, #0xff`.  Declaring `s32 mode;` (or `u32`) and passing `(u8)mode`
produces the mask, and the two-arm `if` that sets both `delta` and `mode` is
what b56 if-converts into `movlt`/`movge`/`rsbge`.

Also: a struct copy `q = *(Vec3 *)(g + 0x24);` written as a *declaration*
initialiser is hoisted above the guard; assigning it inside the block keeps
retail's `add r0, r3, #0x24` / `ldm` / `stm` after the early-out.

### 2.106 A cast pointer subscript is what selects the scaled-index addressing form

ov002 `0x02080d90` compares and clamps a `s32` pair in a loop.  Written as
`*(s32 *)(g + i * 4)` twice, b56 folded the address into a register
(`add r2, r4, r3, lsl #2` then `str [r2]`); retail keeps
`lsl r2, r3, #2` + `str [r4, r2]`.  Spelling the *read* as `((s32 *)g)[i]`
(array subscript on the cast pointer) gives retail's indexed store, and the
whole unit closes.  Same family as 2.100: what matters is the lvalue form, not
the arithmetic.

### 2.107 A type pun on a small array's address keeps it in memory

ov002 `0x0205083c` fills a local `u16 a[3]` and passes elements as `s16`.
`(s16)a[0]` lets b56 promote the array into registers and the frame disappears;
`*(s16 *)&a[0]` takes the address, so the array stays on the stack and the
`strh` / `ldrsh` mix (and the 0x10-byte frame) comes back.  When retail reads
back a halfword it has just stored, the original source used a punning cast,
not a scalar conversion.

### 2.108 Retail's `ls` early-out comes from the for loop, not a hand-written guard

arm9 `0x0201abfc` opens with `cmp r1, #0` / `addls` / `popls` / `bxls`.  Writing
`if (*(u16 *)(a1 + 0) <= 0) return;` before the loop makes b56 fold the unsigned
`<= 0` to `== 0` and emit `addeq`/`popeq`/`bxeq`; `<= 0`, `!(x > 0)`, a `u32`
local and a signed cast all fold the same way.  Dropping the explicit guard and
writing only `for (i = 0; i < *(u16 *)(a1 + 0); i++)` reproduces the `ls`
predicated epilogue exactly (the loop-bound guard b56 generates keeps the
unsigned `<=` form).  Prefer the loop over a redundant guard when the predicate
is `ls`/`hi`.

### 2.109 An inline cast-pointer subscript keeps the `lsl` inside the addressing `add`

arm9 `0x0201e014` loops `ldrh` over four halfwords at `q + 4`.  With a byte
pointer (`*(u16 *)(q + 4 + i * 2)`) b56 promotes the byte offset to its own
induction variable and either bumps it (`add r4, r4, #2`) or hoists a separate
base (`add sb, r7, #4`), both one instruction short of retail's
`add r0, q, i, lsl #1` / `ldrh r0, [r0, #4]`.  Writing the source as
`u32 t = ((u16 *)(q + 4))[i];` -- an index expression on the cast pointer with
the offset folded into it, not a `tp` variable and not a `for`-loop strength
reduction -- yields retail's scaled `add` and the whole unit closes.  Same rule
as 2.106, one step further: the subscript must sit on the punning cast.

### 2.110 Scheduler-sensitive siblings: compute the ternary before the copies

ov002 `0x020d7f74` (sibling of `0x020d149c`) mixes two plain word copies with a
`+/-0x140000` delta on the last store.  Interleaving the delta into the final
store (`*(s32 *)(e + 0x9c) = v.x + (c < 2 ? 0x140000 : -0x140000);`) makes b56
finish the two copies first; retail starts the compare at the top of the block.
Assigning the delta to a local right after the preceding `strh` stores, then
using it in the final `add`, reproduces retail's interleaving.

### 2.111 A two-case `switch` flips the allocation that `||` gives

ov002 `0x02064d58` came out 244 B and instruction-for-instruction right except one
swap: retail keeps the loaded `base` in `ip` and the status word `k` in `r3`, while
`if (k == 0xa4 || k == 0x151)` produces the opposite assignment (`base` in `r3`, `k`
in `ip`).  Re-spelling the same test as

    switch (k) {
    case 0xa4:
    case 0x151:
        ...
        return;
    }

reproduces retail's allocation with no other change.  A disjunction of two equality
tests and a two-case switch share a body but not a register map, so when a unit is
otherwise exact and only two temporaries are transposed, rewrite `||` as a `switch`
(or the reverse) before parking it as a tie.

### 2.112 A switch decision tree follows the parameter's signedness

arm9 `0x02020c50` dispatches `a0` over `0x10`..`0x70`.  Declaring `u32 a0` makes b56
emit the balanced `bhi`/`bhs` tree; retail's `bgt`/`bge` pairs mean the parameter is
`s32`.  The signed-counter rule that governs `u16` loop increments applies to switch
lowering too, and the two trees differ only in branch mnemonics, so check signedness
first when a switch unit's tree shape (the `cmp`/`bgt`/`bge` nesting and the case
block order) is otherwise right.

### 2.113 Copy a two-word header with a struct assignment, and name a rotated sum

arm9 `0x02017bfc` starts by copying `bounds[0]` / `bounds[1]` into an 8-byte
stack local.  Two separate stores (`r[0] = bounds[0]; r[1] = bounds[1];`) make
b56 interleave load/store (`ldr`/`str`/`ldr`/`str`), and hoisting both into
`u32 b0, b1` temporaries batches them but swaps which register holds which.
Declaring a two-word struct and copying it whole -- `typedef struct { u32 a, b; }
Range; ... r = *bounds;` -- gives retail's `ldr r2, [r5]` / `ldr r1, [r5, #4]`
/ `str r2, [sp]` / `str r1, [sp, #4]` in one step.  The same unit's trailing
`*(u32 *)(prev + 4) + (u32)(prev + 0x10) == bounds->a` came out with the three
temporaries rotated (`r1`/`r0`/`r2` instead of `r2`/`r1`/`r0`); naming the sum
(`u32 t = ...; if (t == bounds->a)`) reproduces retail's assignment.

### 2.114 A bumped counter's element store must index with the old count

arm9 `0x0200880c` appends `i` to a counted list: `n = *a1; *a1 = n + 1;
a1[n + 1] = i;`.  Written that way b56 forwards `n + 1` into the index and emits
`lsl r0, r0, #1` / `strh r4, [r5, r0]` on the already-incremented value.  Retail
computes `add r0, r5, r1, lsl #1` from the *old* count and stores with a `#2`
offset, which only comes back when the address is spelled explicitly from `n`:
`*(u16 *)((u8 *)a1 + n * 2 + 2) = i;`.  When a post-increment chain leaves the
index arithmetic on the wrong value, re-spell the store address.

### 2.115 Declare a loop counter before a value that lives across the loop

ov003 `0x020af6d4` keeps the `func_0200fd40` result for use after the `i < 0x1b`
loop and counts with a `u16 i`.  With `s32 rv = ...; u16 i;` b56 hands `r5` to
the counter and `r6` to `rv`; retail has the opposite.  Swapping the two
declarations (`u16 i;` first, then `s32 rv = ...;`) reproduces
`mov r5, r0` / `mov r6, #0`.  When a unit differs only in which of two
callee-saved registers holds the loop counter and which holds the across-loop
value, swap their declaration order before trying anything else.

### 2.116 Declaration position, not first use, steers callee-saved allocation

ov002 `0x020c26a8` was byte-identical except that `base`, `e` and `r` came out in
`sb`/`r8`/`r7` instead of retail's `r7`/`sb`/`r8`.  The three pointers are first
defined in the same order either way, so only the declarations differed: b56
hands the free callee-saved registers out in *declaration position* order, and
here that was `1st -> sb, 2nd -> r8, 3rd -> r7`.  Retail wanted `base` in `r7`,
so `base` had to be declared last:

```c
    u8 *e;
    u8 *r;
    u8 *base;      /* declared last, defines first */
    ...
    base = B;
    e = base + 0x2e4 + off;
```

Free the initialisers from the declarations when you do this; with
`u8 *base = B;` first the allocator follows the definition order instead.  Two
`u32`/`s32` flags in the same function can be swapped the same way (§2.115).

### 2.117 mwccarm aborts with "<string not found>" on a multi-line call

Splitting a call's argument list across two source lines makes b56 fail with
`<string not found>` at that line and abort, even though the same call compiles
when it is on one line.  Keep every call on a single line, however long.  The
same file with the arguments rejoined (ov006 `0x0204da24`) compiles normally.

### 2.118 `push {r0, r1, r2, r3}` at entry means the function is variadic

b56 emits that four-word home area only for a `...` function; a plain six-argument
function keeps its parameters in registers.  ov002 `0x0205837c` is proof: with
six named parameters the prologue is `push {r4, r5, r6, r7, lr}`, and only
`void f(u32 a0, ...)` produces `push {r0,r1,r2,r3}` followed by the real frame.

Inside such a function `((s32 *)&a0)[n]` reads the home area, and constant `n`
folds into direct `sp`-relative loads exactly like retail
(`ldr r1, [sp, #0x14]` for a1, `[sp, #0x20]` for a4).  The one thing that does
not come back is `a0` itself: taking its address makes b56 reload it from the
home slot, where retail still has it in `r0` (`mov r5, r0`).  Park any unit whose
only residue is that reload.  This repo has no `stdarg.h`, so the naming below is
`(s32 *)&a0` and every call must stay on one line (§2.117).

### 2.119 Indexing a `u16` buffer: write the base, then index it

ov005 `0x0205a7ec` copies `buf[1..n]` out of a stack array.  With `buf[i + 1]`
b56 folds the `+1` into the byte offset and emits `add r2, sp, #2` / `lsl r0, r3,
#1` / `ldrh r1, [r2, r0]`.  Retail instead builds `&buf[i]` and loads one element
on: `add r2, sp, #0` / `add r0, r2, r3, lsl #1` / `ldrh r1, [r0, #2]`.  Spelling
the access as `((u16 *)((u8 *)buf + i * 2))[1]` reproduces retail exactly.
Whenever an array read differs only in where the constant lands, write the base
pointer plus a separate `[1]` (or `[k]`) subscript instead of folding the
constant into the index.

### 2.120 A reloaded table word means the source kept it in a variable

ov003 `0x020b13d8` stores `*(u32 *)(table + 8)` into a struct field right after
passing it as an argument.  Writing the load twice makes b56 emit two loads
(`ldr r3, [r0]` … `ldr r1, [r1, #8]`); retail loads once into `r6` and reuses it.
Give the value a `u32 n;` local, assign it before the call, and use `n` in both
places — that also reclaims the `r6` the register set needs.
