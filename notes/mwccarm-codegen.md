# mwccarm codegen levers (ATOJ)

An index of source-shape levers verified against this cartridge, kept reusable.
Function names appear as evidence pointers. Nothing here predicts the allocator:
a lever is an axis to move, and every candidate still has to be compiled and
byte-diffed before it counts.

Toolchain for this repo: CodeWarrior `mwccarm` 1.2/sp2, `-O4,p`, ARM mode,
`struct` alignment 4. The ROM is 2004-12-30 vintage; CodeWarrior builds of that
era behave like the 2004 `b56` line in some places (see §6).

## 0. How to work

* A function counts only when the whole rebuilt ROM still hashes to
  `8d555a80db47278475ffdfdf808fe80dbd7c48cf`. Run `python -m ninja sha1` after
  every accepted unit; nothing else is proof.
* Compile one unit, diff it, fix the **first** structural divergence, then look
  at colours. Do not chase the diff count.
* Read register *roles*, not register numbers: which web owns which register and
  in which order it is written. A whole-function rename is one problem, not N.
* Two spellings that "should" be identical frequently are not (see §2, §3). The
  cheapest experiment is usually reordering statements, not rewriting logic.
* Keep the wall list (§6) current. A wall is a function whose shape is
  source-reachable but whose instruction selection is not, together with the
  evidence that says so.

## 1. Symptom → lever quick table

| What the diff shows | Try first | § |
|---|---|---|
| Whole function a consistent rename | declaration order (reverse for callee-saved locals), then statement order of first use | 3.1 |
| Retail has `add rX, base, #off` then `[rX]`, you fold into `[base, #off]` | separate pointer local used twice; re-check operand count of the base | 2.1 |
| You emit one `add` retail does not have | collapse the pointer into the store expression | 2.1 |
| Retail loads the module base global twice, you load it once | write the global expression twice instead of caching it in a `u8 *b` | 2.2 |
| Retail re-loads a field right after storing it | write the field twice; do not cache the value | 2.3 |
| You emit `movhs`/`movlo`, retail `movge`/`movlt` | the compared operands must be signed (`s32`) | 3.3 |
| Retail branches, you get `movcc`/`strcc` predication (or reverse) | `return a \|\| b;` branches; nested `if (..) return 1;` predicates | 4.1 |
| `and r0, rN, #0xff` at the *call site* vs one hoisted narrow copy | declare the callee's parameter narrow (`u8`), let the call convert | 3.2 |
| Retail stores with post-index `[rX], #imm`, you use `add` | only reproduced when the stride exceeds the store width and the step is its own statement | 4.2 |
| Your function is 8 to 16 bytes shorter, an argument copy is missing | retail keeps a fourth value live; see §3.4 | 3.4 |
| `strh`/`strb` immediate split into `#0x3000` + small offset | the source constant is a value, not a literal; use a `u32 off` local | 2.4 |

## 2. Addressing and memory shapes

### 2.1 Materialised vs folded pointers

`*(T *)(base + off)` folds `off` into the load/store when it fits. Retail
materialises it (`add rX, base, #off` then `[rX]`) in a subset of functions,
mostly read-modify-write of one flag word where the *same* computed address
feeds both the load and the store. A local `T *q = (T *)(base + off);` used for
both halves is the strongest C-side lever; when that still folds, the function
belongs in §6. Evidence: the `func_ov002_0204d09c` five-function set (folds
here), the `func_ov002_020633e0` set and `func_ov002_0207b5f4` (materialise
here).

### 2.2 Count the base loads

Retail often re-reads `data_XXX_02102070` mid-function. Caching it in a local
removes the second `ldr [pc]`/`ldr [rX]` pair and shortens the function by 8 to 16
bytes. Writing the global expression twice restores it. Evidence:
`func_ov002_0206e024` matched only after the second spelling;
`func_ov002_0209e44c` needs the same.

### 2.3 Store-then-reload

Retail sometimes stores a field and immediately re-loads it
(`strb r0,[r4,#2]; ldrb r0,[r4,#2]`). That is the compiler, not `volatile`:
write the dereference twice rather than holding a local.

### 2.4 Literal constants vs values

A constant that is not encodable as one `mov` comes from the literal pool. If
retail splits an offset into `#0x3000` plus a small immediate instead of using a
pool word, the source had the offset as a value (a `u32 off` local), not as a
literal in the expression.

### 2.5 Byte copies

`ldrb/ldrb/strb/strb` with `[rX], #1` post-index is not reachable from C here;
every spelling tried folds to offset addressing or `add` plus copy. See §6. A
2-word adjacent copy is reachable as a struct assignment
(`*(P2 *)dst = *(P2 *)src;`), which batches both loads and then both stores.

## 3. Register allocation

### 3.1 Order decides colour

Callee-saved locals that live across calls are numbered from the last
declaration backwards; statement order of first demand decides the rest.
Permute declarations before touching anything else. When the whole trio is one
register low, the missing pressure is usually a value retail kept live across
the region (§3.4), not a spelling error.

### 3.2 Narrow callee parameters

Declaring an extern as `void f(u8)` or `void f(s32)` moves the conversion to the
call site (`and r0, r4, #0xff`, `mvn r2, #0`). Declaring it wide makes
CodeWarrior hoist one narrowed copy of the caller's parameter. Evidence:
`func_ov002_020e4e2c`, `func_ov002_020788a8`.

### 3.3 Signedness picks the condition code

`s32` operands produce `movge`/`movlt`; `u32` produces `movhs`/`movlo`. Evidence:
`func_ov002_02080eac` differed by exactly those two instructions until both the
load and the parameter became signed.

### 3.4 Register-shift recipes that failed here

Adding `(void)param;`, declaring an extra unused temp, and re-ordering only
declarations did not move a one-register-low trio in eight attempts on the
`func_ov002_0204d09c` and `func_ov002_020a542c` sets. Do not spend more than a
handful of attempts on this class; record it and move on.

## 4. Control flow and shifts

### 4.1 Branch vs predication

`return !(a) || (b);` emits a real branch chain; the equivalent early-return
`if` ladder emits `movcc` predication. Evidence: `func_ov002_0207a93c` matched
only in the `||` form.

### 4.2 Loop shape

A `for (i = 0; i < n; i++) { *p = v; p += k; }` keeps the counter increment
before the pointer bump, matching retail. Post-index stores appear when the step
is larger than the store width and is written as its own statement
(`*(u32 *)p = v; p += 8;` gives `str r,[r0],#8`). Evidence:
`func_ov002_02086f7c`.

### 4.3 Switches

Case body layout follows source label order, so write the labels in the order
retail's blocks appear. A dense switch whose low cases are absent can lower to a
compare chain; a `case` ladder returning a constant lowers to a range check plus
an equality chain. Evidence: `func_ov002_0205078c`.

### 4.4 Signed division by a power of two

On a 32-bit value this lowers to `add rX, rX, rX, lsr #31; asr rX, #1`. Writing
`/ 2` on an `s32` produces it; `>> 1` on the same value does not.

## 5. Data symbols

* `extern E data_X[]` plus `&data_X[i]` gives the pooled address form; an
  `extern u32 data_X;` gives a direct load.
* A read-only table indexed by a byte offset (`data_X[k * 2]`) with an `s16[]`
  declaration produces `ldrsh`. Evidence: `func_ov002_0204f770`.
* Tables of structs use `mla rD, index, stride, base` when the stride is not a
  power of two (`0x158`, `0x1c`, `0x38`, `0x44`), and a scaled index otherwise.

## 6. Known walls

Recorded so the same ground is not covered twice. Each one was swept across
source idioms; none is a skill gap.

* Materialised flag RMW. `add rA, rB, #off; ldr rV, [rA]; orr/bic; str [rV]`,
  and C always folds. Swept: deref, pointer local, index, split-RMW, `volatile`,
  u64-mask launder (two forms). Blocked: `func_ov002_020633e0`, `02063820`,
  `02063c5c`, `0207a93c` (tail), `0209e44c`/`020ace70` (tail), `0204c824`
  (tail).
* Folded halfword address in `return (*p += 1);`. `func_ov002_02072324` wants
  `add r1, r0, #0xa; ldrh r0, [r1]; add r0, r0, #1; strh r0, [r1]`, while C
  folds the base into the load/store (`ldrh r1, [r0, #0xa]`, two accesses, then
  a reload for the return). Same class as the wall below. Swept: pointer local
  declared inline and separately, separate assignment.
* Predicated if/else with a re-materialised else address.
  `func_ov002_0204d488` tests `*(u16 *)(b + 0x30048) >= 0x1f`, stores 0 when
  true and otherwise re-loads and increments. Retail builds the test address as
  `add r0, r2, #0x30000; ldrh r1, [r0, #0x48]`, but the else branch loads
  `0x30048` from the pool, adds it to `b`, and does a fresh `ldrh`/`add`/`strh`
  instead of reusing the tested value. C CSEs both spellings into one address
  and keeps the tested value (`addlo r1, r1, #1`). Swept: `> 0x1f` vs `>= 0x1f`,
  both spellings in both branches, split load/store, `volatile` pointer. Not
  solved.
* Two-load/two-store byte copy with `[rX], #1`. Blocked: `func_02006be0`,
  `02006c10`, `02006cd0`, `02008a60`, `0200e99c`, `020080d0`, `0200e8f4`,
  `02010248`.
* Pre-indexed writeback `ldr rV, [rB, rI]!`. Blocked: `func_ov002_0207808c`,
  `020781b4`, `02086020`, `02085ff8`, `02062b20`, `0206308c`,
  `func_02033cb0`.
* One-register-low temp trio with an inverted branch. Still blocked:
  `func_ov002_0204d09c` only. The rest of that set (`0204d0e0`, `0204d12c`,
  `0204d1e8`, `0204d22c`, `0204d270`, `0204d2b4`, `0204d2f8`) is solved; the
  missing lever was the live callee result in §10. The `0x5c`/`0x60`
  state-machine sets are solved too, see §9.
* Signed word-fill loop. `func_020048e8` wants `cmp r1, ip; stmlt r1!, {r0};
  blt` (signed test, store-multiple writeback). C gives `cmp` plus `bxhs` plus
  `str r0, [r1], #4`. Swept: pointer local, `(s32)` casts on both sides,
  struct-valued fill, byte stepping. The `lt` test and the `stm` writeback are
  separate missing levers.
* MMIO address from the literal pool. `func_02024ae4` (`0x027c0000`) wants
  `ldr r0, [pc, #8]`; C builds the constant in a register instead (16 B against
  20 B). Same shape as the pool-address wall in §2, but with an absolute
  address rather than a symbol.
* `mrc` result register. `func_0202607c`: `asm { mrc p15, 0, v, ... }` allocates
  the destination as a temp (`r1`) plus a copy; retail reads straight into `r0`.
  The mask `0xfffff000` also has to come from the pool, while C canonicalises
  `& 0xfffff000` to `mov`/`rsb`/`and`.
* Adjacent bare calls get a frame. `func_0202612c` (two `void` calls and nothing
  else) wants `stmdb sp!, {lr}` + `bl` + `bl` + `ldm`; C inserts `sub sp, sp,
  #4` and `add sp, sp, #4`. That is reserved-stack alignment, not source.
* `b` local in the family clone colours r4/r5 backwards. `func_ov002_020c08e8`
  is §9's template plus one extra store (`*(u8 *)(b + a0 + 0x14a) = 0;`) and
  needs `b` in `r4`, `p` in `r5`. All declaration orders tried put `b` in `r5`.
  Membership test for §9 is therefore: no `b` local, or an exact colour match.

### Levers found while cracking the §9 clones

* Compound assignment fixes evaluation order. `s32 d = A - B;` makes CW load `B`
  first and `A` second (right to left). Writing `s32 d = A; d -= B;` loads `A`
  first, which is what retail does; this matched `func_ov002_0206394c` on the
  first try after the two-load form failed.
* Offset before scaled index. In a loop body, `*(u16 *)(t + i * 8 + 6)` and
  `*(u16 *)(t + 6 + i * 8)` are not the same unit: the second form matches
  `func_ov002_02063168`, and it also moves the hoisted constants up one register
  pair (`r2`/`r3` instead of `r1`/`r2`).
* Family clones with an extra store keep `B` un-cached. `func_ov002_020daa24`
  matches only when the template's `B` macro is used twice instead of a `u8 *b`
  local; with the local the pointer lands in `r2` and the slot product in `r1`,
  the reverse of retail. `020c08e8` is still blocked and needs the local but the
  opposite colouring.
* `r = 1; *(u8 *)(p + 0xd1) = r;` matches when state and flag are the same
  constant and the variable is stored after the assignment (`mov r3,#1; strb
  r3,[r4,#0xd1]`), unlike the plain §9 template which stores the state first.

## 7. Workflow

```sh
# one candidate: compile, diff reloc-aware, accept only on exact match
python build/t10.py --try <name>            # try_unit(...) from Python

# gates, in order
python -m ninja sha1                        # whole-ROM digest
python build/t10.py --check arm9 ov002 ...  # delink link-state sanity
python -m ninja report                      # progress numbers
```

`build/cdiff.py <mod> <name>=<file.c>` compiles one file and prints a
reloc-aware instruction diff against retail without touching the tree, which is
the fastest way to read a residue. `build/auto.py` exposes `set_module`, `FUNCS`
(addr → name, size), `SYM` and `disasm(addr, size)`.

## 9. Solved: the per-slot state-machine family (21 functions)

Twenty-one `0x5c`/`0x60` functions share one skeleton: `mul` by `0x158`, a
`*(u32 *)(p + 0xd8) & mask` gate, `strh 0` to `p + 0x66` (and sometimes
`p + 0x6a`), a call taking `*(u8 *)(b + n + 0x2e6)`, `*(u8 *)(p + 1)` and a
literal, then a state byte at `p + 0xd1`, then `return`.

What moves the residue is exact statement order at the tail, plus keeping the
`0x2e6` byte as a named local declared after the `s32 r = 0;`:

```c
s32 func(u32 a0)
{
    u8 *p = B + 0x2e4 + a0 * 0x158;
    s32 r = 0;
    u8 k = *(u8 *)(B + a0 * 0x158 + 0x2e6);
    if (*(u32 *)(p + 0xd8) & MASK)
    {
        *(u16 *)(p + 0x66) = r;      /* r, not 0 */
        /* *(u16 *)(p + 0x6a) = r;   on the two-field variants */
        CALLEE(k, *(u8 *)(p + 1), CODE);
        *(u8 *)(p + 0xd1) = STATE;   /* state store BEFORE r = 1 */
        r = 1;
    }
    return r;
}
```

`r = 1;` before the state store, or inlining `k` into the call, both invert the
branch into an early return and shift the trio one register down. Matched set:
`020a542c`, `020e28b8`, `020eac6c`, `020f1a94`, `02099f40`, `0209b8ec`,
`020a16d8`, `020a17bc`, `020a385c`, `020a67c0`, `020ac768`, `020b2a04`,
`020b2a64`, `020b5c44`, `020b9b78`, `020b9bd8`, `020bcbc4`, `020c9be8`,
`020cb768`, `020d1314`, `020d2da0`.

## 10. Solved: the `0x274` per-slot flag setters (8 functions)

Eight ov002 functions index a global table (`B + 0x274`, stride `0x1c`) and set
a flag word at slot offset `0x18`: `0204d1e8`, `0204d22c`, `0204d270`,
`0204d2b4`, `0204d0e0`, `0204d12c`, `0204d2f8`, `0204cb44`. Three levers carry
the set; each was a §6 wall before it was found.

### 10.1 A live callee result keeps `r0` occupied

Retail's temp trio starts at `r1`, not `r0`, whenever the helper's return value
survives to the epilogue. Caching the call in a local and returning it makes the
allocator keep `r0` live and shifts everything up one register:

```c
void func(s32 a0, s32 a1, s32 a2)
{
    u32 r;
    u8 *t = B + 0x274;
    r = func_ov002_0204d350(a0, a1, a2);
    *(u32 *)(t + 0x18 + a0 * 0x1c) |= 0x8;
    return r;                       /* the lever: r0 must stay live */
}
```

Dropping the local (`func(...);` then the RMW) starts the trio at `r0` and the
unit misses by one register, which is exactly the §6 one-register-low class.

The lever is not tied to the flag RMW: `0204d178` and `0204d1b0` are the same
shapes with a single `*(u8 *)(t + a0 * 0x1c + 2) = a2;` instead of the OR, and
they also need the live result (`mov r1, #0x1c` rather than `r0`).

### 10.2 `p = t; p += i;` materialises an address C would otherwise fold

Retail emits `add rB, rT, rI` and then offsets from `rB`. Writing `p = t + i` as
one expression lets the compiler pick `mla` plus a folded immediate, one
instruction shorter. Assigning the base and then incrementing keeps the index
live for a second use, so the address really is materialised:

```c
    i = a0 * 0x1c;
    t = B + 0x274;
    p = t;
    p += i;                        /* add rB, rT, rI, no mla */
    if (a2 == *(u8 *)(p + 2)) ...
    if (a1 == *(u8 *)(t + i)) ...  /* second use keeps both live */
```

The same function also wants the index computed before the base is loaded,
which only happens once the pointer increment is spelled out.

### 10.3 An inline loop bound is re-read every iteration

Retail reloads the count (`ldrh r0, [sb, #2]`) at the loop tail. Caching it in a
local hoists a single load and the loop shrinks by 8 bytes:

```c
    for (i = 0; i < *(u16 *)(a0 + 2); i++, a1 += 0x10) { ... }
```

`func_ov002_0204c824` is the residue of this family: it wants `mla r0, ...` (the
slot pointer in `r0`) followed by `add r3, r0, #0x18`, while C puts the slot
pointer in `r3` and folds `0x18` into the load/store. Swept: pointer local,
`u32 *p[6]`, `f = p; f += 6;`, split `*f = *f & ~0x400`, separate base and slot
pointers. Same materialisation wall as §6.

## 11. Solved: the ov003/ov005 stub and thunk set (54 functions)

Whole modules of small units are reachable in one pass once the shapes are
recognised. Fifty-four units went in together and none needed a lever beyond
writing the obvious C.

### 11.1 `mov r0, #0; bx lr` is `return 0;`

Twenty-three ov003/ov005 functions are 8-byte constant returns. A `void` body
compiles to a bare `bx lr`, so the return type has to be non-void for the `mov`
to appear.

```c
u32 func(void)
{
    return 0;
}
```

### 11.2 `ldr ip, =callee` plus argument moves is a tail call

`mov r0, #1; mov r2, r0; mov r1, #0; bx ip` is exactly
`return callee(1, 0, 1);`. A plain last-statement call tail-calls too, but
returning the result keeps the `u32` prototype honest and picks up the value
form of the thunk.

### 11.3 A small table index in front of a thunk has to stay scaled

`func_ov003_020641c8` loads `data_ov003_020dc744[a1]` and passes it as the third
argument; the `<< 2` must be written on the index, as in the sibling
`func_ov003_020641e4`, or the compiler folds the scale into the load and the
argument order changes.

### 11.4 A `== 0` test picking between `sub` and `rsb` is a predicated pair

`func_ov003_020555b4` compares the selector against zero once and emits both
arms predicated. Write the `== 0` arm first (`(a1 << 5) - 0x1000`), then the
`else` arm as `0x1000 - (a1 << 5)` so it becomes the `rsb`.

## 12. Solved: Thumb units (8 functions)

The BIOS stubs in arm9 are Thumb rather than ARM: `symbols.txt` marks them
`kind:function(thumb,...)` and the retail bytes are `swi n` plus `bx lr` as
16-bit instructions. The build needs no extra flags, because the source opts in:

```c
#include "types.h"
#pragma thumb on
void SoftReset(void)
{
    asm { swi 0x00 }
}
```

That reproduces the 4-byte stub exactly, and mwldarm places it at the delinked
address like any other complete unit. Eight went in this way: `SoftReset`,
`WaitByLoop`, `HuffUnCompReadByCallback`, `Div`, `IntrWait`,
`LZ77UnCompReadNormalWrite8bit`, `RLUnCompReadByCallbackWrite16bit`,
`BitUnPack`, one file per stub with its own `swi` number.

### 12.1 Thumb units whose address is 2 mod 4 cannot be placed

Every Thumb stub that matched sits at a 4-byte-aligned address. The ones at
`4n+2` (`Halt` `0x020040e6`, `Sqrt`, `CpuSet`, `GetCRC16`, `CpuFastSet`,
`IsDebugger`, `RLUnCompReadNormalWrite8bit`,
`LZ77UnCompReadByCallbackWrite16bit`, `VBlankIntrWait`) all link with the whole
ARM9 image shifted four bytes: the object's `.text` is a clean 4 bytes with no
recorded alignment, yet mwldarm pads the section to a word boundary, the entry
point moves from `0x2004800` to `0x2004804`, and every following byte differs.
Diffing the linked ROM puts the first difference at the stub itself. Not solved.

### 12.2 The three Thumb struct initialisers stay uncoloured

`func_0201fcc8`, `0201fce4` and `0201fd00` write nine words of a fixed layout
(`0x1000` at one slot, `0`, `a1`, `a2`, `-a1` at the others). They are 4-byte
aligned and link fine, but the store order does not fall out of the obvious
sequence and the sizes already match, so the residue is instruction order inside
the Thumb block, not placement.

## 13. Prior art

Levers above are cross-checked against, and reusable from, these:
`tangosdev/sm64ds-decomp` `notes/mwccarm-codegen.md` (statement order colours
temps; the materialisation wall and its build dependence; struct-copy load
batching), `fuddlesworth/pokebw2` `docs/matching.md` (narrow parameters change
caller scheduling; direct-global accesses instead of cached; pointer-reference
count flips added-vs-folded), `ReGame-Labs/dwds_decomp` (1.2/sp4 `-O4,p`),
`6elphegor/acww-decomp` (1.2/base, Thumb), `InusualZ/mhtri-decomp` (subscript vs
pointer-sum operand order), and the `decomp.wtf` MWCC guide.

## 2.1a Address materialisation: a measured rule (2026-11-20)

The fold-vs-materialise question in 2.1 can be decided by a small experiment
instead of guessing. Compiling one-statement functions with the project's flags
gives:

| source | mwcc output |
|---|---|
| `((u32 *)p)[9] |= 2;` | `ldr r1,[r0,#0x24]` / `orr` / `str r1,[r0,#0x24]` |
| `*(u32 *)(p + 0x24) |= 2;` | identical, fully folded |
| `((u32 *)(p + 0x20))[1] |= 2;` | `add r1,r0,#0x20` / `ldr r0,[r1,#4]` / `orr` / `str r0,[r1,#4]` |

So the rule is:

- A **single constant offset** - whether written as a byte offset or as a large
  array index, since `9 * 4` folds to `36` - is folded into the load/store
  offset field. The base register is never materialised.
- A **constant base plus a scaled index** materialises the base with `add` and
  folds only `index * sizeof(T)` into the access. mwcc splits the total
  (`0x20` in the `add`, `4` in the offset); it does not move the whole constant
  into the `add`.

Consequence for matching: if retail shows `add rX, base, #K` followed by
`[rX]` with offset 0 (as `func_0201ce4c` and `func_0201cb84` both do for K =
0x24), no C spelling of a constant-offset RMW reproduces it. Eleven spellings
were tried on `func_0201ce4c` - plain statements, bound `u32 *`, `u32`-typed
address, explicit `*q = *q & ~1`, struct member, two 1-bit bitfields,
`((u32 *)p)[9]`, `((u32 *)(p + 0x20))[1]`, `(u16 *)p + 0x12`, `volatile`
qualified, and an index variable pinned to 0 - and every one folds to 132 B
where retail is 136 B. Treat retail's `add #K` + `[rX]` as a build-time
artefact, record it, and move on.

One practical trap found while bisecting: `((u32 *)p)[9] |= 2;` compiles in
isolation but makes this mwcc emit an internal error (`<string not found>`,
exit 1) inside a larger function body. `compile_text` returns `b""` for it, and
`fastmatch variants` reports COMPILE-FAIL, so always sanity-check a
COMPILE-FAIL spelling in isolation before concluding the body is wrong.

## 2.6 Compiler-runtime units in the ready list (2026-11-20)

A shape scan of the 217 ready arm9 units finds **12 units / 4,948 bytes of mwcc runtime**, not game code:

- `func_02039630` (912 B) - int64
- `func_02039b54` (896 B) - softfloat
- `func_02037f0c` (732 B) - softfloat
- `func_0203a584` (492 B) - softfloat
- `func_02007e7c` (400 B) - softfloat
- `func_0201fe0c` (344 B) - softfloat
- `func_0202aea8` (304 B) - softfloat
- `func_0201a100` (288 B) - softfloat
- `func_0200e0ec` (172 B) - softfloat
- `func_0202e674` (164 B) - softfloat
- `func_0201ff64` (156 B) - softfloat
- `func_02024b04` (88 B) - softfloat

Signatures used to spot them: `clz`/`umull`/`smull`/`smlal`/`umlal` (soft-float and 64-bit multiply,
e.g. the `__mulsf3`-style routine at `0x0203a584` that pushes r0-r3 around a call to read the FP state
register and does `umull` on the mantissas), and `mov ip, #0x200000` followed by `cmn ip, rX, lsl #1`
(the sign-normalising trick of a 64-bit signed compare, `__cmpdi2`).

This corrects an earlier note in `build/walls.txt` that called `func_02039630` (912 B) a "generated
fixed-point dispatch family": it is the 64-bit compare runtime. That unit, `func_0203a584` and the other
ten are **not decompilation targets** - their retail bytes came from an assembly runtime, so per the
project rules they must not be generalised into C. Exclude them when picking work; the decompilable
ready pool is 205 units / 35,120 bytes, and the largest real prize is `func_0201a57c` (808 B).

## 2.7 Frames and stack usage: measured (2026-11-20)

Experiments with the project's flags, each compiled in isolation:

| source | mwcc |
|---|---|
| `void f(u32 *p){ u64 z; z = (u64)*p; *p = (u32)z; }` | 12 B, no frame - the `u64` is eliminated |
| `void f(u32 *p){ double d; d = 0; *p = (u32)d; }` | 32 B, `push {r4,lr}`, no frame padding |
| `void f(u32 *p, u32 a, u32 b, u32 c, u32 e){ *p = a; }` (5 declared, `e` unused) | 8 B, no frame |
| `void f(u32 *p, ... 9 args){ *p = i; }` leaf | 12 B, no frame, stack args at `[sp+0]` |
| same 9 args, but the body calls something | 28 B, `push {r4,lr}`, stack args at `[sp+0x18]` |
| `void f(u32 *p, S8 s)` with `typedef struct {u32 a,b;} S8;` | 28 B, `push {r0,r1,r2,r3}` - by-value struct spill |

Conclusions:

1. **Unused locals never leave a frame.** A declared `u64` or `double` that the body does not need
   disappears entirely, frame included. A surviving stack reservation therefore means something
   *uses* it.
2. **Frames come from calls.** A leaf function with stack arguments reads them at `[sp+0]` with no
   prologue at all; the `stmdb sp!,{lr}` + `sub sp,sp,#4` pair only appears once the function is
   non-leaf. An unexplained 8-byte frame in a function with no visible `bl` therefore implies a
   **call that was inlined**, which matches the two units in `build/walls.txt` that carry one
   (`func_02017954`, `func_0201779c`, `func_02021c70`): their bodies are complete and their logic
   reproduces exactly, minus the frame.
3. **`push {r0,r1,r2,r3}` as the first instruction means the function takes a 16-byte struct by
   value**, not that it has locals - that is the parameter home area.
4. **Unused stack parameters cost nothing.** Declaring extra parameters to "explain" a frame does
   not work; mwcc drops them.

## 2.8 What is not decompilable in the ready list

Classifying the 217 ready arm9 units by shape gives:

| class | detect | units | bytes |
|---|---|---|---|
| IRQ/asm helpers | `mrs`/`msr`/`swp` | 7 | 664 |
| soft-float runtime | `clz`/`umull`/`smull`/`smlal`/`umlal` | 11 | 4,036 |
| 64-bit compare runtime | `mov ip,#0x200000` + `cmn ip,rX,lsl #1` | 1 | 912 |
| by-value struct entry | `push {r0,r1,r2,r3}` | 5 | 468 |

That is **24 units / 6,080 bytes that cannot become C** (the first three are compiler or SDK
runtime emitted from assembly; the fourth is decompilable but needs the exact struct type in the
signature). The realistic candidate pool is **193 units / 33,988 bytes** - not the 217/40,260 that
the ready count suggests. Filter with these signatures before picking work; three of the largest
units in the list (`02039630`, `0203a584`, `02024b04`) are in the non-C classes.

## 2.9 Switch statements: the bisection tree (2026-11-20)

A retail function that begins by comparing one register against a series of constants with
`bhi`/`blo`/`bhs`/`beq`, where several branches land on the *same* `mov rK,#imm; bx lr`, is a
**`switch`**, not an if-chain. mwcc emits a balanced binary search over the sorted case set, so the
tree shape is fixed by the case values alone and reproduces exactly. Writing the same values as an
if-chain never matches: it produces a linear compare ladder.

Two rules decide whether the *blocks* match:

1. **Case-block emission order follows the order the labels are written in the source.** Retail's
   constant blocks appear in source order, which for the decode tables found so far is **ascending by
   return value**, not by case value. `func_ov003_02051e58` needs
   `0x40, 0x10, 8, 8, 4, 4, 4, 2, 2, 0x20, 0x20, 1` and `func_ov002_0205fe90` needs
   `0x100, 0x200, 0x400, 0x800, 0x1000`. Reordering case labels changes nothing but the block
   addresses, so a 348-byte unit lands within five branch-target diffs of a match purely by getting
   this order right.
2. **A `switch` whose arms `return` merges the returns into one shared block per value.** That is the
   same shared-tail shape the `goto` lever produces: `switch (v) { case a: case b: return 1; } return
   0;` gives one `mov r0,#1; bx lr` block reached by every matching branch.

Together these make switch-shaped units cheap: identify the tree, read each leaf's constant, write the
cases in ascending-return order. Six units, 1,692 bytes, matched this way in one session including a
three-member family that is byte-identical apart from the function name.

`build/ready.py <mod> --frameless` plus a scan for "at least 3 branches to one target and at least 2
bhi/blo" finds them: 30 were sitting in the ready pool across arm9 and the overlays.

## 2.10 Address materialisation: the pooled form, solved (2026-11-20)

Measured directly, compiling minimal functions with the project's flags.

`*(u16 *)(b + 0x897e)` compiles to a **split** address - `add r0, b, #0x8900` + `[r0, #0x7e]` - in
every one of these spellings, which were all tried:

    *(u16 *)(b + 0x897e) = *(u16 *)(b + 0x897e) + 1;
    ((u16 *)(b + 0x8900))[0x3f] = ...;            /* u16 index of an offset base */
    ((u16 *)b)[0x44bf] = ...;                     /* u16 index of the raw base  */
    *(volatile u16 *)(b + 0x897e) = ...;

Retail for the same site uses the **pooled** form - `ldr r0, =0x897e` + `add r1, b, r0` + `[r1]`. The
two spellings that produce it are both of the form *"the full address lives in its own variable"*:

    volatile u16 *p = (volatile u16 *)b + 0x1e6c ... (bound pointer)   -> pooled
    u32 a = (u32)b + 0x897e;  *(u16 *)a = *(u16 *)a + 1;              -> pooled

and the placement of that variable decides whether the pool load is predicated or hoisted: computed
**inside** the `if` body, the load is `ldrlo`/`ldrne` exactly like retail; computed before the `if`, it
is unconditional. Retail's pattern in one function is therefore

    if (*(u16 *)(base + 0x897e) < 0x10) {   /* read: split form, add+offset */
        p = (u16 *)(base + 0x897e);          /* address: pooled form, inside the branch */
        *p = *p + 1;
    }

So the rule to try first when a RMW is 4-8 bytes short and retail shows a pooled constant: give the
address its own pointer inside the branch, while the *test* keeps the plain expression. Beware the
knock-on effect on register choice - the local pointer can land in `ip` where retail uses `r3`, which
is a separate allocation tie.

## 2.11 Jump tables and case order (2026-11-20)

A dense `switch` (values packed into a small range) compiles to `addls pc, pc, rK, lsl #2` followed by
one `b` per case, with a `b` before the table as the default. Two things decide a match:

1. **Cover the range.** Three cases inside 0..7 make mwcc emit a compare ladder instead; listing
   `case 0: case 2: case 3: case 4: case 6: break;` alongside them produces the jump table retail has.
   A grouped `case ...: break;` still costs nothing at run time but changes the table.
2. **Block order follows the order the labels are written, not the case values.** `func_02014e64`
   needs its three bodies in the source order `7, 1, 5` - every other permutation of the same three
   cases lands at the right size with a handful of swapped branch targets. Brute-force the factorial
   (three cases = six orders, cheap) rather than reasoning about it.

`func_ov002_02060730` shows the easy end of the class: a seven-case mapping with one statement per
case matched with zero diffs first try.

### Nested inversion, third confirmed use

`if (x == A) { small } else if (x == B) { small }` often produces the *wrong* predication polarity.
Writing it as

    if (x != A) { if (x == B) { ... } } else { ... }

puts the small if-converted block in the taken path and branches to the other one - which is retail's
usual layout. This decided `func_ov002_0207b5f4`, `func_ov002_020e4450` (down to 84/84 with one
`bx`/`bxeq`) and `func_ov002_02061850`, where the requested variant matched on the first try after the
direct form missed by 4 bytes.

There are **57 jump-table switch units** in the ready pools (`addls pc, pc` scan across arm9 and every
overlay), so this is currently the highest-yield class in the project.

## 2.12 The jump-table scanner and what it wrongly attracts (2026-11-20)

`build/jt.py` walks every ready unit (`auto.FUNCS` minus `src/<mod>/*.c`) and reports each one whose
body contains `addls pc, pc`, with counts of `mov rN,#imm`, `b` and `bl`. It currently reports **125**
units - more than twice the 57 I estimated by eye. Filter with `$NF=="bl=0"` and `mov>=3` for the
high-yield class: a switch that is nothing but a value-to-value map.

Best single hit so far: `func_ov002_02060798`, 256 B, a 23-case mapping over `0..0x16` that matched
with **zero diffs on the first attempt**. Read the table, write one `case N: r = V; break;` per entry
in ascending case order, list the zero-cases only if the table needs the range covered, done.

Two traps the scanner also surfaces, both dead ends:

* **Fragments.** Some "ready units" are the middle of a large function: the disassembly starts with a
  `mov sb, r0` / `cmp r0,#0` and pops `{r4,r5,r6,lr}` that were never pushed. `020271d4`, `0201c59c`
  and `02032f90` are all this. Skip anything whose first instruction is not a prologue-shaped one and
  that references `r8`/`sb`/`sl`/`fp` without having set them.
* **Register-write soup.** Literal-pool hardware addresses (`0x04000204`, `0x027fff60`) plus a long
  `strh` run at fixed offsets. No C shape reproduces it quickly.

A profitable non-jump-table shape the scan also turned up: `func_02024674` (184 B) is an
alignment-padding **fall-through cascade** - `switch (a & 3)` with `*p->cur++ = 0;` in cases 1, 2 and 3
with no `break`, so case 1 performs one store, case 2 two, case 3 three, aligning `a` down to a
multiple of 4. That reproduces the blocks exactly; only the field-address materialisation
(`add r2,r0,#4` plus a double load of `p->lim`) is still missing.
