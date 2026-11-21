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

## 2.13 Hand-decoding still beats generation for jump tables (2026-11-20)

I tried to automate the pure-map class end to end (`build/jtgen.py`): find the `addls pc, pc`, read the
table, map each entry to the `mov rD,#imm` it lands on, sort blocks by address (= source order, which
is the one degree of freedom), emit `case N: r = V; break;` and compile it. Run over all 125 units it
produced **zero** additional matches. `func_ov002_02060798` remains the only unit that shape covers.

Two reasons the automation has nothing to chew on, both worth recording:

* **Most tables are not maps.** The blocks do stores, calls or several operations, so the generator
  bails ("block not pure"). That is the common case, not the exception: 118 of 125.
* **The ready pool contains fragments.** A unit whose first instruction is `bx lr`, or that reads
  `r8`/`sb`/`sl`/`fp` without having set them, or whose disassembly ends mid-epilogue, is the middle of
  a larger function and cannot be matched as a unit. Filtering the 125 for "real prologue (`push`/
  `stmdb`/`cmp` in the first instruction) + table in the first 10 instructions + real return at the
  end" left exactly **two**.

### A filter that works

That same filter found `func_0200ee58` (arm9, 256 B), which matched in four attempts: a 16-way table
where only cases 4, 7, 10, 12 and 15 do anything, each `if (func(imm) != 0) return 1; break;`, with
`return 0;` after the switch. Every one of the five bodies is the *same* shape with a different
constant, so the switch is really a table of `(case, argument)` pairs - read the arguments off the
`mov r0,#imm` in each block and the source writes itself.

The one iteration that mattered: the last case must be written with `break;` like the others, not
`return func(...) != 0;`. The `break` form lets mwcc if-convert it into `addne/movne/ldmne/bxne`
against the fall-through `return 0`, which is exactly what retail has; the `return` form emits a
branch and costs 4 bytes.

### Generator status

`build/jtgen.py` is kept because it is correct and cheap; when a genuine map turns up it produces the
source in one call. `build/jtrun.py` runs it across `build/jt.txt` and reports how many units it could
parse (currently 2, both fragments). Do not spend a session on the jump-table vein expecting bulk
yields - it is now a tooling-backed trickle of ~1 unit per pass.

## 2.14 The address-materialisation wall (2026-11-20)

This is the single most common reason a small unit stalls now, and it is worth recognising on sight
because no source form I have found defeats it.

Retail frequently computes a base+offset address into a register and then accesses through that
register:

    add r2, r0, #0x24        addne r1, r0, #0x108        add r1, r0, #4
    str r1, [r2]             ldrne r0, [r1]              ldr r0, [r1]
    str r0, [r2, #8]         orrne r0, r0, #4            bic r0, r0, #0x80000000
                             strne r0, [r1]             str r0, [r1]

mwcc in this project's configuration always folds the same access into the addressing mode instead:

    str r1, [r0, #0x24]      ldrne r0, [r1, #0x108]      bic r0, r0, #0x80000000
    str r0, [r0, #0x2c]      orrne r0, r0, #4            str r0, [r1, #4]
                             strne r0, [r1, #0x108]

Same instruction count, same bytes except one arm — and unclosable. Forms tried on
`func_02018318` (arm9, 24 B), which is *one store* from matching:

* a local `u32 *q = (u32 *)(a0 + 0x24)` used twice (and once with `q[2]`);
* a `u8 *q` with `*(u32 *)(q + 8)`;
* a struct typedef with `q->a` / `q->c` member access;
* loading the value into a variable first, then storing;
* `volatile u32 *`;
* integer rather than pointer arithmetic (`u32 q = a0 + 0x24;`);
* spelling the address inline with no variable at all.

Every one produces the folded form. `func_0201f980` (28 B), `func_0203684c` (40 B) and
`func_02024b5c` (48 B, predication instead) fail in the same way.

**Practical rule: if the only difference is `[rM]` versus `[rM, #imm]`, stop.** Two attempts to
establish whether the shape is reachable is enough; record it in `build/walls.txt` and move on. This
is the same category as the register-allocation ties in the main notes, and it is not a sign that the
source is wrong - mwcc at this version simply does not materialise here.

## 2.15 A census of what is actually left (2026-11-20)

`build/census.py` filters the ready pool down to units that can plausibly be matched as units at all:
last instruction is a real return (`bx lr`, `ldm sp!`, `pop`), and no `r8`/`sb`/`sl`/`fp` register is
read without the function having set it up. Result: **201 of the ~2,430 unmatched units**. The rest
are fragments of larger functions and cannot be matched in place.

Distribution: 42 units under 64 B, 60 at 64-127, 47 at 128-191, 15 at 192-255, 13 at 256-319, 6 at
320-383, 4 at 384-511, 14 at 512-575.

The list is in `build/ready_clean.txt`, smallest first. Everything examined from it so far (twelve
units) failed on the materialisation wall (§2.14) or on compiler-generated ldm/stm copy loops, which
is a different but equally unreachable shape: retail's 3-4 interleaved `ldm r0!,{r2,r3,ip}` /
`stm r1!,{r2,r3,ip}` pairs come from consecutive 12-byte struct assignments, and mwcc merges them into
either a counted loop or a frame-carrying memcpy-like sequence, never the straight-line pairs.

## 2.16 Two large targets decoded but not closed (2026-11-20)

### The 13x756 ov002 family (9,828 B)

`build/famsurvey.py ov002` reports `13x756 val 9828 0x0208f860 0x0209a5f4 0x0209c728 0x0209eb18
0x020a8ba0 0x020ad2b0 ...`. I verified the members really are one source: normalising immediates and
register numbers, each member is **185 of 186 instructions identical** to the first (99%), with the
same instruction count. So a match on member 1 is worth 9,828 bytes across 13 units - by far the
largest single prize left in the project.

Decoded structure of `func_ov002_0208f860` (member 1), in order:

* prologue `push {r4-r8,sb,sl,fp,lr}` + `sub sp,#0x44`; the frame holds 8 stack arguments and 6
  live locals, so expect many slots.
* `base = *(u8 **)data_ov002_02102070`; entry stride **0x158**; `sl = base + 0x2e4 + idx*0x158`;
  `t24 = base + 0x1744 + idx*0x38`; `sb = base + 0x1824`.
* `v = *(u8 *)(base + idx*0x158 + 0x2e6)`; `t28 = (v >> 1) & 0xff`; `fp = (s8)(1 - 2 * (v >> 1))`.
* 9 calls: `func_0205bf64`, `func_02034148`, `func_02053468`, `func_0205bee4`, `func_0204f770`,
  `func_020532e4`, `func_0204f6ac`, `func_0203b0c0` (x3), `func_02057068` (3 register + 8 stack
  args), `func_020780f8`.
* an if/else on `*(u32 *)(sl + 0x4c) & 4` selecting between a byte at `t24+8` (==2 / ==0) and two bits
  of `*(u16 *)(sl + 0x50)` (0x10 / 0x20), each adding or subtracting `res` from a s16 accumulator.
* `/360` via the `0xb60b60b67` magic, then `fp * (r7/360)` masked through `asr #0x10` and `& 0xff`.
* conditional negations written with the `mvn`/`mul`/`mov` idiom: `if (x < 0) y = -y` becomes
  `mvnlt r0,#0; mullt r0,r6,r0; movlt r6,r0`.
* a u16 counter loop to 0x64 computing `c = x*w + w; c = x*t3c + t3c; x++; c -= (x*c)>>1;
  c += *(u32*)(sb+8);` with two early exits (`c > 0x30000`, counter > 0x64) and `if (x <= 0) x = 0xa;`
* tail: writes `0x42000000`, `9`, `*(u8*)sl`, `*(u16*)(sl+0x12c)`, idx and three temps as stack
  arguments to `func_02057068`, then `func_020780f8()`, then clears bit 0 of `*(u32*)(base+0x38)`.

A draft exists at `build/cand/big2.c`: compiles to 548 B of the 756, so roughly two thirds of the
shape is wrong or missing - it is a starting point only, not close. This unit wants a dedicated
session; do not expect a family payout until member 1 is byte-exact.

### The 4x196 ov003 family (784 B)

`02066e84 020b8904 020b8e54 020dbad4`. Reconstructed to **164/196 B, 34 differing slots**. The logic
is not in doubt; three mechanics block it:

1. **Pooled RMW address.** Retail does `ldrlo r0,[pc]=0x897e; addlo r1,r4,r0; ldrhlo; addlo; strhlo`
   - the constant is pooled *and* the address materialised in r1. Writing
   `u16 *p = (u16*)(b + 0x897e); *p = *p + 1;` gives the folded form instead.
2. **Split vs folded condition addresses.** The same field read in the `if` condition must use
   `add r0,r4,#0x8900` + `ldrh r0,[r0,#0x7e]` (split), i.e. spell it `b + 0x8900 + 0x7e`, while the
   body uses the pooled `b + 0x897e`. Two different spellings of the same address in one function.
3. **Base register `r3` vs `ip`** on the shared `b + 0x8900` base used by the 5-argument call.

CORRECTION (same day): I first concluded that `bl` to a cross-module symbol did not verify through
`fastmatch` (`got bl #0x2066ed0`). That was wrong. Relocations are masked **per instruction slot**,
so any unit whose instruction stream is shifted by one instruction during compilation reports a
`bl` difference at every subsequent slot as well - the unmasked offset is a symptom of the shift, not
a cause. When the source shape is right the `bl` slot is masked correctly and compares clean. Do not
chase this again.

## 2.17 Callees that return a 64-bit value (2026-11-20)

`func_02018bbc` (arm9, 44 B) matched only after a wrong first reading. Retail is:

    mov r2, r0
    add r0, r1, #1
    ldr r1, [r2, #4]
    bl func_0203b2cc
    lsl r0, r1, #0x10
    lsr r0, r0, #0x10

The `ldr r1` is the second argument, and then `r1` is *read after the call* - which a normal compiler
cannot do, because r0-r3 are caller-saved. The explanation is not that the callee preserves r1: it is
that `func_0203b2cc` is an **8-byte function returning a u64**, so the result arrives in `r0:r1` and
the code keeps only the high word:

    extern u64 func_0203b2cc(u32, u32);
    u64 v = func_0203b2cc(a1 + 1, *(u32 *)(a0 + 4));
    return (u16)(v >> 32);

Three spellings of that (named temp, inline expression, and a `u16` return type) all match - the
lever is the u64 return type of the *callee*, not the expression.

**Generalisation worth using as a search filter:** if a value loaded into `r0`-`r3` *before* a `bl`
is used *after* it, the callee returns a 64-bit value and the source is taking its high half. mwcc
would otherwise have to spill to a callee-saved register, so the tell is a missing `push {r4,...}`.
The callee's symbol size is the confirmation: an 8-byte function in `symbols.txt` returning a u64.

I also corrected an error in section 2.16: the `bl` mismatch I saw there was not a harness bug. Relocs
are masked per instruction slot, so any unit whose instruction stream is shifted by one instruction
reports a `bl` difference at that slot; when the shape is right the slot masks cleanly.

## 2.18 Harvesting the helper trees (2026-11-20)

I had been pushing walls, notes and tools **into** the scout/worker/delta trees every session and never
reading anything back out of them. Doing that once was worth 23 units and 3,084 bytes - more than the
previous four sessions combined.

**The helper trees accumulate finished work that nobody claims.** Verified against retail from this
tree's ROM, then installed through `reg.py`:

* delta: ov003 `0x0205e028` (172), `0x02081ecc` (172), `0x0204e5bc` (64) - 408 B.
* scout + worker: 20 arm9 units, 2,676 B - `0x02004e4c` 140, `0x0200eac4` 136, `0x0200ec0c` 64,
  `0x0200f0a4` 156, `0x0200f6cc` 100, `0x02012454` 140, `0x0201484c` 232, `0x02014c5c` 68,
  `0x02014ca0` 148, `0x02014ed8` 144, `0x02015250` 124, `0x020153ec` 232, `0x02016030` 100,
  `0x02016790` 136, `0x02016934` 116, `0x02016ee8` 132, `0x02017a88` 112, `0x0201a07c` 132,
  `0x0202b33c` 96, `0x0202e718` 168.

**One fifth of their "MATCH" candidates were wrong.** Of 25 candidates that `fastmatch`/`var3` call
MATCH, `reg.py` rejected 5 outright on the ROM digest (`0x020065b0` 144, `0x0200f638` 148,
`0x020146e0` 160, `0x020262cc` 112, `0x02029768` 84). That is the pool/literal class the AGENTS notes
warn about: the instruction stream matches with relocations masked but the constant pool differs. Never
install a helper's candidate on its own claim - run `reg.py` here.

**The helper trees are badly stale.** scout and worker are on `4024535` with 1,295 sources here-1,376;
delta is on `266619a` with 1,363. They are matching units that were installed days of commits ago and
searching pools that have since changed. Anything they hand over must be re-verified in this tree, and
their queues must be re-pointed after every release.

### The harvest loop (use it every session)

    python build/harvest.py          # verify every helper cand/*.c against THIS tree's retail
    python build/install_helpers.py  # reg.py each verified candidate, reject on ROM mismatch

Both files are in `build/`. Run them before concluding a session has nothing to show; the helpers
produce more than the session log suggests.

## 2.19 Stale symbol tables produce phantom rejects (2026-11-20)

The five candidates that `reg.py` rejected while harvesting (§2.18) were **not** the pool/literal class.
All five scored `fastmatch` MATCH and failed only because their extern names came from an older
symbol table. The instruction streams were correct the whole time:

| unit | size | wrong name in candidate | real symbol |
|---|---|---|---|
| `0x02029768` | 84 | `data_0204abe8` | `data_0204abec` (4 bytes off) |
| `0x020065b0` | 144 | `data_2042a68` | `data_02042a68` (missing zero) |
| `0x0200f638` | 148 | `data_203cb6c`, `data_203cb6e` | `data_0203cb6c`, `data_0203cb6e` |
| `0x020262cc` | 112 | `func_02048c8` | `func_020248c8` (missing digit) |
| `0x020146e0` | 160 | `func_02062d84` | `func_ov002_02062d84` (overlay call) |

The first symptom is misleading: `reg.py` prints only the last log line, and `ninja`'s final line is
`build stopped: subcommand failed`. To see the real cause, reproduce `try_unit` by hand with
`t10.configure_build()` and print the whole log - it says
`mwldarm.exe: Undefined : "func_02062d84"` or a plain compile error.

Two lessons:

1. **A symbol 4 bytes off still masks as a reloc.** `data_0204abe8` vs `0x0204abec` changes the linked
   image, but `fastmatch` masks the pool word, so it reports MATCH. Only the digest catches it.
2. **arm9 calls into overlays.** `relocs.txt` records `kind:arm_call ... module:overlay(2)`; the extern
   must use the overlay's symbol name (`func_ov002_02062d84`), not an arm9-style name. Grep
   `relocs.txt` for `to:<addr>` when a `bl` target has no arm9 symbol.

`build/lint_cand.py` now checks every `extern` name in a candidate against all symbol tables
(arm9, each overlay, itcm, dtcm) before anything is installed. Run it on every helper hand-over.

All five are now installed: +648 bytes.

## 2.20 A quarter of the "ready" pool is not functions (2026-11-20)

`build/ready_clean.txt` (201 entries) was built by filtering for units with no external references.
That filter says nothing about whether the unit is a *function*. Checking the first and last
instruction of every entry splits the pool:

* **140** start with a prologue (`push`/`stmdb`, or `sub sp`) and end with `bx lr`/`pop`/`ldm` -
  real functions. Saved as `build/ready_funcs.txt`, smallest first.
* **61** do not: they are epilogues (`add sp,#4 / pop {r4,r5,lr} / bx lr`), loop tails
  (`stmlt r1!,{r0}; blt`), mid-function slices (`ldrhne r0,[r1,#-1]; andne ...`) or data
  (`udf #0xffff`). Saved as `build/ready_frags.txt`.

Fragments cannot be matched from C, ever: there is no function boundary to compile. dsd split them
out because they are branch targets. Do not spend iterations on them, and prefer
`ready_funcs.txt` when picking work.

### Three instruction-selection ties found this session

Each of these is one instruction away and resisted every rephrasing tried (4-6 variants each).
Recorded so nobody re-walks them:

* `0x0202612c` (20 B) - `stmdb sp!,{lr}; bl; bl; ldm sp!,{lr}; bx lr`. mwcc always inserts
  `sub sp, sp, #4` between the push and the two calls (8-byte call alignment); retail's 4-byte
  frame has none. Scan of every arm9 unit of 12-32 B found **no other unit with this shape**, so it
  is a veneer or a non-C stub, not a wrapper someone wrote in C.
* `0x02018318` (24 B) - every instruction matches except the first store: retail materialises
  `add r2, r0, #0x24` and stores `[r2]`; mwcc folds it to `str r1, [r0, #0x24]`. Pointer variables,
  `q[0]`, `*(q+2)`, struct fields and `volatile` all fold the same way (volatile instead spills to
  the stack and is worse).
* `0x020048e8` (20 B) / `0x020269ec` (24 B) - the word-fill / word-copy loops. Retail is
  `cmp; stmlt r1!,{r0}; blt` (predicated single-register `stm`, condition at top). mwcc emits a
  rotated loop with `bxhs lr` and a post-index `str r0,[r1],#4`. `#define`-free rephrasings
  (`while`, `for`, `do`, u32/u8/s32 counts) all rotate.

## 2.21 The 13x756 family: structure map for `func_ov002_0208f860` (2026-11-20)

Working draft is `build/cand/big3.c`. It is **not** a match (676 differing bytes) but the whole body is
now decoded, so the next session starts here instead of at the disassembly. 189 instructions, 756 B,
frame `sub sp,#0x44` on top of `push {r4-r8,sb,sl,fp,lr}`.

Call order and the stack slots retail keeps:

| slot | holds |
|---|---|
| `sp+0x20` | `a0` |
| `sp+0x24` | pointer `base + 0x1744 + a0*0x38` |
| `sp+0x28` | `t28 = (v>>1) & 0xff` |
| `sp+0x2c` | `t2c = *(sl+0x128)`, later `t30 = t2c*sg` |
| `sp+0x30` | `acc = t34 * hx` |
| `sp+0x34` | out-param `t34` |
| `sp+0x38` | second `func_0203b0c0` result |
| `sp+0x3c` | `t3c` |

Sequence:

1. `v = *(u8*)(base + a0*0x158 + 0x2e6)`, `sl = base + 0x2e4 + a0*0x158`, `sb = base + 0x1824`,
   `t28 = (v>>1)&0xff`, `sg = (s8)(1 - 2*(v>>1))` (retail builds this as `rsb` after `lsl/sub/lsl/asr`).
2. `hx = (func_0205bf64(t28) - 0x14) << 16`, `w = 0x760000`,
   `gX = func_02034148(0xffffbf8c)`, `X = gX + 0x157c` **before** the next call, which passes
   `gX` (not `X`) and `t2c = *(sl+0x128)`; result is truncated to `s16` by `lsl #16 / asr #16`.
3. `r = func_0205bee4(0x1e, sl)`; then either `(*(u32*)(sl+0x4c) & 4)` -> `*(u8*)(t24+8)` tested
   `==2` (add) and `==0` (sub), or `(*(u16*)(sl+0x50) & 0x10/0x20)` -> add/sub. All four are if-converted
   `addne/lslne/asrne` with `(s16)` wrapping.
4. `t30 = t2c*sg`; `func_0204f770(t30, (u8)(s16)(sg * ((ang<<8)/360)), &t34)` - the `/360` is the
   `0xb60b60b7` magic on `ang<<8`.
5. `if (t28==1) w = -w;` `if ((s32)t34<0) hx = -hx;` `if (t34==0) { if (*(s32*)sb<0) hx = -hx;
   t30 = func_020532e4(hx, w, &t34, t30); }` then `acc = t34 * hx` (retail multiplies `t34` by
   **whatever is in r0**, i.e. `hx` when the branch was taken and the call's return otherwise).
6. `y = 0x2d0000 - *(sb+8); if (y<0) y = 0;` `fa = w - *(sb+4);`
   `t = func_0204f6ac((y>>8) * ((X>>8)<<1));` `t3c = t<<8;` `x = func_0203b0c0(t3c, X);`
7. Loop (`0x1f4`..`0x244`): accumulators. `r2 += X` and `r1 += t3c` per iteration with
   `i = (i+1)&0xffff`, `x++`, `c = *(sb+8) + r1 - (x*r2)/2` (signed `/2` via
   `add r3,r3,r3,lsr #31; asr #1; rsb`). Exit: `i > 0x64` (`bhi`), `c > 0x30000` (`bgt`, which jumps
   **back to `add r2,r2,r4`**, i.e. the continue target is mid-body because of the accumulator).
   After the loop: `if (x <= 0) x = 0xa;`
8. Tail: `fa = func_0203b0c0(acc, x); fb = func_0203b0c0(fa, x);` then the 11-argument
   `func_02057068(fa, fb, t3c, 0, 0xffffbf8c, t28, 0x42000000, 9, *(u8*)sl, *(u16*)(sl+0x12c), a0)`
   (three register args + eight stacked words), `func_020780f8()`, then
   `t = *(u32*)(base+0x38); t &= ~1; *(u32*)(base+0x38) = t; return t;`

**What blocks it:** register pressure. `big3.c` spills `a0` to `fp` and gets a 0x34 frame where retail
has 0x44 and keeps `a0`/`t24`/`t28`/`t2c` on the stack. The next step is to name locals to match retail's
slot assignment exactly (one variable per slot above), especially making `t24` and `t28` stack-resident
by writing them once and reading them twice.

### `autorun.py` is exhausted for arm9

`python build/autorun.py 45 120 arm9` reports `already tried 465`, `bailed 92`, `testing 0`,
`generated matches: 0/0`. The generator has no untried patterns left in that size band; more units need
hand-written reconstruction from `build/ready_funcs.txt`.

## 2.22 Two levers that turned a near-miss into a match (2026-11-20)

`func_0201fc3c` (60 B) matched after two edits that are worth reusing:

* **Signed compare decides `ldrge` vs `ldrhs`.** `p[2] >= p[3]` on `u32*` emits `ldrhs`; retail has
  `ldrge`, so the source compares signed values (`(s32)p[2] >= (s32)p[3]`).
* **Hoisting a field load breaks the early-return predication.** Writing `u32 lo = p[0];` at the top
  made mwcc load `p[0]` into r4 *before* the compare. Retail loads only `p[2]`/`p[3]` up front,
  predicates `ldrge r0,[r0,#4] / popge / bxge`, and loads `p[0]` after. Declaring `lo` but assigning
  it after the `if` reproduces that exactly.

### `func_020124e0` (68 B) - one swapped pair, parked

A bit-test helper: `p = func_0200faa8(a0); if (!p) return 0; return p[a0/8] & (1 << (a0%8));`
with `s32 a0` (`/8` and `%8` are the signed-div sequence `asr #2 / add ... lsr #29` and
`lsr #31 / rsb / ror #29`). Everything matches except the schedule: retail computes the **mask**
before `ldrb`, mwcc computes it after. Four formulations (index/mask variables, `>>3`+`&7`, `/8`+`%8`,
mask-first) all schedule the load first. Recorded, not chased further.

### `func_02028920` / `func_02028958` (56 B pair) - the COMPILE-FAIL pair is now explained

`p->a28 += p->a34; p->sub->cb40(p->sub, p->a2c, <old a28>);` where `cb40` is `[sub+0x40]` (and `[sub+0x3c]`
for the second). It compiles fine from C - the old COMPILE-FAIL was a bad candidate. It does not match:
retail's frame is `push {r4,lr}` (uses r4 for `sub`) while mwcc emits `stmdb {lr}` + `sub sp,#4`, and
retail materialises `add lr, r0, #0x28` instead of an offset load. Both are the alignment/address-materialisation
ties already catalogued in 2.14 - not a new class.

## 2.23 The materialisation cluster: four units blocked by one fold decision (2026-11-20)

Four freshly decoded units all fail on the same single decision. Retail materialises `base + small
offset` into a register and accesses through it; mwcc folds the offset into the load/store immediate.

| unit | size | retail | mwcc |
|---|---|---|---|
| `0x020187b0` | 72 | `add r2, r4, #0xc; ldrh r1,[r2]; sub; strh r1,[r2]` | `ldrh r1,[r4,#0xc]; sub; strh r1,[r4,#0xc]` |
| `0x02018818` | 72 | same shape, `+1`, offset `0xa` | folded |
| `0x0201f26c` | 68 | `add r2, r4, #0x118; ldr r1,[r2]; add; str r1,[r2]` | folded |
| `0x02018318` | 24 | `add r2, r0, #0x24; str r1,[r2]; str r0,[r2,#8]` | `str r1,[r0,#0x24]` |

Bodies (all verified by hand, none installed):

* `0x020187b0`: `if (func_02018b90(p)) return 0; *(u16*)(p+8) = func_02018bbc(p, *(u16*)(p+8));
  *(u16*)(p+0xc) -= 1; return 1;`
* `0x02018818`: identical with `func_02018ba4`, offset `0xa`, and `+= 1`.
* `0x0201f26c`: `if (*(u32*)(p+0x118) == 0) { if (func_0201cde8(p) == 0) return 0; }
  *(u32*)(p+0x118) += 1; return 1;`
* `0x020289f4`: order + `|= 4` on `p+0xc`, `p+0x2c = p+8`, zero `p+0x34`/`p+0x32`, store `a1` to
  `p+0x30`, `mov r1,#2`, `bl func_02028af4`. mwcc tail-calls it (`b`) while retail emits `bl` + epilogue,
  which is the second half of this cluster's problem.

### What the matched units say the trigger is

Scanned every matched arm9 unit for `add rX, rY, #imm` immediately followed by an access through `rX`:
**7 hits, all materialising into `ip`**, and the cleanest is `func_02010248`:

    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x158) + a0 * 8 + 0x24;
    *(Blk *)a1 = *(Blk *)p;      // ldrb r2,[ip],#1 ... loop

There the pointer must live in a register because it feeds a **post-increment copy loop**. Try as I
might, a plain RMW (`*q = *q - 1`, `q[0]--`, `*(u16*)(p+0xc) -= 1`, `volatile u16 *q`, `((u32*)p + 3)`)
never produces the materialised form. So the trigger is not "pointer variable" but "the pointer is
consumed by something that requires a register address" - and these four units have nothing like that.

**Conclusion:** these four need a source shape whose RMW is expressed through a register-held address
for a reason the compiler cannot fold. Until one is found they stay parked; do not re-derive the bodies,
they are above.

## 2.24 The folded-offset class is 13.5% of the remaining arm9 work (2026-11-20)

Follow-up to 2.23. Chain of evidence:

1. Four unrelated units (`0x020187b0`, `0x02018818`, `0x0201f26c`, `0x02018318`) fail on one decision:
   retail materialises `add rX, rY, #imm` and accesses through `rX`; mwcc folds the offset into the
   memory operand. Every formulation tried folds (`*q = *q - 1`, `q[0]--`, `&p->field`, `volatile`,
   `((u32*)p + n)`, declaring the pointer early so it must survive a call - the last one *does*
   materialise, but early and in a callee-saved register, giving `push {r4,r5,lr}` instead of retail's
   `push {r4,lr}`).
2. Three more units with the same shape but a global base were decoded and tried
   (`0x02015040` clear bit, `0x02015068` set bit, `0x02015524` increment + compare) - all fold
   identically, including the variant that re-reads the global twice.
3. Searching all **matched** units for the pattern finds 7, every one of them materialising into `ip`
   in a **post-increment copy loop** (`func_02010248`: `ldrb r2,[ip],#1`). No matched unit materialises
   a base+const for a plain RMW.
4. Counting the unmatched arm9 set: **72 of 535 (13.5%) contain the pattern.**

Conclusion: this looks like code built with a different fold heuristic from the object files the other
1,407 units came from (SDK/library objects, or a different 1.2 service-pack revision). It is not a
source-shape puzzle we can win by rephrasing - six candidates across four units and five formulations
say so. Since flags and compiler version are fixed by project rules, these 72 are effectively parked.

**Working queue is now `build/ready_funcs2.txt`**: `ready_funcs.txt` with the folded-offset class and
everything already in `walls.txt` filtered out. That is the list to pick from.

## 2.25 `goto` labels defeat if-conversion and fix block order (2026-11-20)

`func_02029fb0` (84 B) matched only after the two `return`s were expressed as explicit labels:

    s32 func_02029fb0(u32 a0, u32 a1)
    {
        u8 buf[0x24];
        if (func_0202a3a0(buf, a0, a1) == 0)
            goto zero;
        if (func_0202a060(buf) != 0)
            goto one;
    zero:
        return 0;
    one:
        return 1;
    }

Retail is `cmp; beq zero; add r0,sp,#0; bl; cmp; bne one;` then the `zero` block, then `one`. Writing the
same two `if (…) return 0;` / `if (…) return 1;` statements instead makes mwcc **if-convert** the small
bodies (`addeq sp; moveq r0,#0; ldmeq; bxeq`) and pick the opposite block order, which no amount of
rephrasing fixed. The `goto tombstones` force real branches and give mwcc the layout retail has.

Use this whenever a unit's failure is `beq/bne` in retail against `…eq`-predicated blocks in your build,
or when the two exit blocks come out in the wrong order. Variant that also matches: nest the second
call inside the first `if` and `goto one` from inside it.

Corollary for reading diffs: a first-difference of `want beq | got addeq sp, sp, #…` is this class, not
a size or arithmetic problem.

## 2.26 `func_02034608` (84 B): a perfect body with a swapped register pair (2026-11-20)

Decoded body, correct to the instruction, differing only in which callee-saved register holds what:

    s32 func_02034608(u32 *p, u32 a1, u32 a2)
    {
        u32 n = a2;
        if (p[2] + n > p[1])
            n = p[1] - p[2];
        func_02034494(p[0] + p[2], a1, n);
        p[2] = p[2] + n;
        return 1;
    }

Retail: `mov r5, r0` (p in r5) then `mov r4, r2` (n in r4). mwcc: `mov r4, r0` then `mov r5, r2` -
the two are exchanged and nothing else differs. Five formulations slide the same way: named local,
in-place parameter update, `u32 *q = p;` copy, assignment-after-declaration, `n = a2` vs `a2`. This is
the register-allocation tie class from the top of these notes; parked, body kept here.

Also examined from `ready_funcs2.txt` and left alone: `0x02021ec4` (88 B) and `0x02027080` (88 B) both
pack eight arguments into one register with a chain of `orr …, lsl #n` before a call, and
`0x02032aa8` (88 B) does two unaligned u16 reads out of its own argument block. All three need the exact
argument order of the callees first; they are not guesses worth making without it.

## 2.27 Harvest #3: two more 124-byte units (2026-11-20)

`build/harvest.py` found `0x02019974` and `0x02019aa0` (124 B each) in the scout tree. Both installed
through `reg.py`; `0x02019974` needed the familiar symbol repair (`data_204662c` -> `data_0204662c`,
missing zero) which `build/lint_cand.py` flagged before install.

Running total from harvests: **28 units, 3,980 bytes** that were sitting finished in the helper trees
and never claimed. The loop is `harvest.py` -> `lint_cand.py` -> `install_helpers.py` (or `reg.py` one at
a time) at the start of every session; it takes a minute and has paid every time it has been run.

## 2.28 The harvest was blind to every overlay (2026-11-20)

Root cause of three sessions of "the helper trees are drained": `build/harvest.py` and
`build/install_helpers.py` resolved candidate sizes through `build/auto.py`'s `FUNCS`, and that table
holds **arm9 only** (1,365 entries). Every `func_ovNNN_*` candidate was silently skipped. delta's
reports had been claiming ov003/ov004/ov005/ov009 matches for hours and none of them could ever surface.

Fixed by building symbol tables from `config/ATOJ/arm9/symbols.txt` *and* every
`config/ATOJ/arm9/overlays/*/symbols.txt` (two line formats in use - match both:

    NAME kind:function(arm,size=0x1c) addr:0x02015f40
    NAME kind:function size=0x1c addr:0x02015f40

Result on the first run: **16 verified candidates, 2,084 bytes**, of which 14 installed (1,820 bytes;
the two rejects are the fastmatch-passes/digest-fails class - `func_ov004_02061f58` 108 B and
`func_ov005_02056908` 156 B). Installed: arm9 `0x0201ced4` 160; ov003 `0x0204cd8c` 100, `0x02052100` 96;
ov004 `0x020500cc` 136, `0x0205a6b0` 148, `0x0205ca30` 84, `0x0205e274` 64, `0x020624e0` 140;
ov005 `0x0204cf6c` 108, `0x02051b90` 140, `0x020569c0` 208, `0x0205a8dc` 172;
ov009 `0x0204dc70` 140, `0x0204e8c0` 124.

`install_helpers.py` now also repairs the stale-symbol class automatically before comparing: any
`data_XXXXXXX`/`func_XXXXXXX` with seven hex digits that is not in the symbol tables is retried with a
zero after the prefix (`data_204662c` -> `data_0204662c`). That fixed one candidate without manual work.

**Lesson:** any tool that resolves units through `auto.py` is arm9-only. Check the module coverage of the
tool before concluding a source is exhausted.

## 2.29 The two "digest rejects" were symbol names again - 264 more bytes (2026-11-20)

Both candidates `install_helpers.py` rejected after harvest #4 turned out to be buildable; the diffs were
entirely in the literal-pool words (`andeq …` lines at the end of the listing), which is what a wrong
symbol reference looks like once relocations are masked.

* `func_ov005_02056908` (156 B): used `data_02079900` - the **arm9 spelling** of an overlay symbol. The
  real name is `data_ov005_02079900`. Renamed, linked, digest match.
* `func_ov004_02061f58` (108 B): `func_0206256c` -> `func_ov004_0206256c`. Same class.

So the rule from 2.19 holds for overlays too: an overlay unit must reference **its own module's** symbol
names (`data_ovNNN_*`, `func_ovNNN_*`) even when the address is shared with arm9. `lint_cand.py` catches
both before install if it is run; `install_helpers.py` now also repairs the seven-digit form
automatically, but the module prefix needs judgement and is worth checking by hand when a link fails.

Harvest total after 4 rounds: **30 units, 4,244 bytes**.

## 2.30 The `goto` lever is the shape, not the instance (2026-11-20)

`func_0202a004` (92 B) is `func_02029fb0`'s sibling: same 0x24-byte buffer, `func_0202a3a0(buf, a0, a1)`,
and two early exits that share a tail block. The `&&` formulation if-converts into a single conditional
and reorders the blocks; the explicit `goto` form reproduces retail instruction for instruction. Both
callers of `func_0202a3a0` in arm9 are now matched, so the family is complete at two members.

Two levers, both structural rather than stylistic:
* early-return chains that share a tail need labels (`goto zero; ... zero: return 0; one: ...; return 1;`)
* `if (f(x) != 0 && g(x) != 0)` and `if (f(x) != 0) { if (g(x) != 0) { ... } } return 0;` are *not*
  equivalent to mwcc - only the label form emits both `bl`s unconditionally and joins the exits.

`build/ready_funcs2.txt` had drifted: it still listed 12 units that were already installed, which is why
`func_02029fb0` and `func_0202a004` appeared as candidates. Regenerated from `src/` before this batch;
now 83 entries.

## 2.31 Frame-shift residue: read the prologue, not the body (2026-11-20)

Three family attempts this session all failed the same way, and it is worth naming as a class.

When `diff_summary` reports that *every* offset differs but the pairs are clearly the same instructions
one slot apart, the code is not wrong - the **frame** is. `func_ov003_02066a54` and its two siblings
(3 x 180 B) reproduce retail instruction for instruction after the prologue, but retail is
`push {r4,r5,lr}` followed by `sub sp, sp, #4` while mwcc emits `push {r4,r5,r6,lr}` with no slot.
mwcc found a register for whatever retail spilled; retail's source therefore had one more
**address-taken 4-byte local** than the reconstruction does. That is a property of the original
function's body, and no rearrangement of the loop (`u16 i` + `i % 4`, a `u16 *q` base pointer,
`u32 i` + `i & 3`) recovers it.

The practical rule: **diff the prologue first.** A one-instruction offset at the start with an
otherwise identical body means stop rewriting the body and go looking for the missing local. If no
plausible local exists, the unit is a frame artifact like `0x0202612c` (the 4-byte-frame veneer) and
should be parked rather than ground at.

Same signature, different cause, in arm9 `0x02023df8`: the whole body is offset by one instruction
because mwcc hoists the `cmp` above the saves and drops r4/r5. There the tell is a **dead
`mov r3, r2`** - a value moved into r3 and never read. Dead moves do not come from C, so the 3rd
parameter is used somewhere we have not found; four formulations failed and it is parked.

All three families are in `build/walls.txt` with their full retail sequences and the closest body, so
the helper trees can pick them up with the derivation already done.

## 2.32 Matched-twins: mine the units that are already done (2026-11-20)

Twenty-odd turns were spent deriving bodies from scratch for functions that had a **twin already in the
tree**. `build/twins.py` fixes the target selection: it groups every function in every module by
`(module, size, mnemonic multiset)` and reports the groups that contain both a matched and an unmatched
member. Those are the cheapest possible wins - the C is already written, only constants differ.

Two found on the first run, both installed first try:

* arm9 `0x02024168` (116 B) - twin of the matched `0x0202352c`. Different body shape though: 2 params
  not 3, `data_02048cbc + 0x20000` instead of `data_02048c94 + a1`, but the same
  `if (handle != 0xffffffff) func_02026738(...) else func_020269ec(...)` skeleton.
* ov002 `0x020a9b48` (156 B) - twin of the matched `0x020a663c`. Only the field mask (0x10 not 0x8),
  the callee (`0204d270` not `0204d22c`), three constants (0x2d/0x2e, 5, 0x80) and one absent global
  write differ. The `#define B (*(u8 **)data_ov002_02102070)` idiom carried over verbatim.

An exact mnemonic *sequence* signature finds nothing here - mwcc reorders the prologue between twins
(`mov r3,r2` dead move, compare hoisting), so register order differs while the instruction mix does not.
The **multiset** is the right granularity: same instruction counts, different schedule.

Caveat worth remembering: the twin's source still has to be re-derived where the *shape* differs
(parameter count, the address expression). Substituting constants blindly is right for the operands,
not for the control flow.

## 2.33 The handle-dispatch family is larger than the twins (2026-11-20)

`twins.py` reporting zero does not mean the family is done - it requires identical *size*, and this
family varies. Searching instead for the skeleton itself (a `mvn rX, #0` compare, a `cmp`, a `beq` and
two `bl`s, size <= 260) found **17 unmatched members** across the tree. Two installed first try:

* `0x020241dc` (112 B) - same as `0x02024168` but the base is `data_02048cbc` unshifted, no `+ 0x20000`.
* `0x02023674` (128 B) - 3 params, and the address is a *difference*:
  `data_02048ca0 + a1 - data_02048c9c`. That is the first member where the address expression is not a
  simple base + offset; reading the two `ldr`+`ldr` pairs (globals then dereferences) is what reveals it.

Members so far, all with the same `if (data_02042370 != 0xffffffff) func_02026738(handle, a0, addr, ...)
else func_020269ec(a0, addr, ...)` skeleton: `0x0202352c` (3p, `data_02048c94 + a1`),
`0x02024168` (2p, `+0x20000`), `0x020241dc` (2p, plain), `0x02023674` (3p, difference).

Remaining in the group worth attacking: `0x0202ce70` (160 B), `0x020263e0` (168 B), `0x0202cdc8`
(168 B), `0x02025710` (212 B), plus ov002 `0x02088b0c` (212 B) and ov004 `0x0204c934` (200 B) - the
overlay ones matter because overlay coverage is thin.

`0x02027f74` (116 B) is a different shape (bit set/clear on `0x027ffc388` guarded by a null test,
wrapped in `func_02026350`/`func_02026364`) and stops at a register tie: mwcc keeps the array base in
r0 where retail uses r1, and the else-block comes out one instruction short. PARKED.

## 2.34 Status of the two sweeps, and two more parked units (2026-11-20)

`harvest.py` (arm9 + all overlays, all candidate filenames) and `twins.py` both return **zero** this
session. The helper trees have no finished-but-unclaimed work left, and every matched unit has been mined
for twins. The cheap sources are exhausted; what remains in the handle-dispatch family is the hard half:

* `0x0202ce70` (160 B) and `0x0202cdc8` (168 B) are a spinlock acquire/release pair over the object at
  `0x0204aec0` (`func_02026350`/`func_02026364` bracketing, `func_020263cc` on the retry path,
  `func_020251c0`/`func_0202514c` on the wait-queue). Multi-exit, retry-shaped: worth doing carefully
  rather than quickly.
* `0x020263e0` (168 B) is a 13-call init sequence gated on `*(u16 *)0x027ffc40 == 2`.
* `0x02032ecc` (108 B) takes a stack buffer, `bic`s the address to 4-byte alignment and fills a
  five-field struct before one call - the same address-materialisation class that resists elsewhere.

Two more parked with full derivations in `walls.txt`, and one reusable result: **`smulbb` + `asr #15` +
`add lsr #16` + `asr #16` is a signed `/ 65536`**, not `>> 16`. That distinction is worth remembering -
`(t * 360) / 65536` reproduces retail, `(t * 360) >> 16` does not.

## 2.35 A false MATCH from a failed compile - always guard the compile (2026-11-20)

While probing the ov002 getter/setter family I got a `diffs 0` result that I briefly read as a match. It
was not: the body declared a local *after* a statement

    p += 0xa;
    u32 v = p[0];      /* C89: declaration after statement */

mwcc rejects that, `compile_text` returns `b""`, and `diff_summary` then reports zero differing bytes
because there is nothing to compare. **Zero diffs is indistinguishable from a silent compile failure** -
exactly the `parse_ins`-without-`retail()` trap from §2.2, in a new place.

Rule: before trusting any `diff_summary` result, assert the compile produced code:

    if not fm.compile_text(body, "guard"):        # empty bytes -> the compile failed
        return "COMPILE-FAIL"

`fm.compare()` is safe because it returns False on an empty object; `diff_summary()` is not, and it is the
function I reach for when inspecting *why* something failed. Every "diffs 0" should be confirmed with
`compare()` before it is believed.

## 2.36 The ov002 getter/setter family is a register-tie class, not a derivation problem

Scanning overlays for small unmatched units (size <= 132, <= 2 calls) found **130 candidates**; the ten
smallest are all one-line bit setters/getters over `*(u8 **)data_ov002_02102070` (and one over
`0x027c0000`). Fully derived, none match. Representative case, `0x02062b48` (32 B):

    retail: ldr r0,[pc] ; ldr r0,[r0] ; add r1,r0,#0x28 ; ldr r0,[r1] ; bic r0,r0,#4 ; str r0,[r1]
    mwcc:   ldr r0,[pc] ; ldr r1,[r0] ; ldr r0,[r1,#0x28] ; bic r0,r0,#4 ; str r0,[r1,#0x28]

Same semantics, one instruction fewer, base in r1 instead of r0. Retail materialises the address; mwcc
folds it into both accesses. Tried: direct expression, typed pointer with `+=`, value-into-a-local,
`volatile` pointer, struct-style access, and byte/word pointer mixes - all give the same folded form.

This is the §"Register-allocation ties" class from AGENTS.md, and it explains most of today's dead ends
(`0x02066e84` base add, `0x02053468` base in r5, `0x020241dc`-family base in r1). A whole family of tiny
units can be blocked by one allocation preference, so **derive the body, confirm the instruction mix, and
park** rather than trying more spellings.

## 2.37 A class where only the *register roles* differ (2026-11-20)

After the getter family (2.36) I searched for units that avoid the RMW pattern entirely - unmatched,
store-free, <= 1 call, <= 128 B - and found 30. The three smallest are pinned down exactly, and the
residue is narrower than anything seen so far. `0x0202c430` (24 B):

    retail: ldr ip,[pc] ; ldr r0,[ip] ; cmp r0,#1 ; beq <ldr> ; bx lr
    mwcc:   ldr r0,[pc] ; ldr r1,[r0] ; cmp r1,#1 ; beq <ldr> ; bx lr

**Same instruction kinds, same count, same control flow - only the register numbers differ.** Retail
keeps the address in `ip` (r12) and the value in `r0`; mwcc uses `r0`/`r1`. This is not a derivation
error: `while (data_0204ad00 == 1);` is certainly the source, and a local `volatile u32 *p = &data;`,
a literal-address form and an unqualified local pointer all compile to the same r0/r1 allocation.

Two other members of the same sweep are not code at all: `0x0201fd00`, `0x0201fcc8`, `0x0201fce4`
decode to nonsense (`movwhs`, `smlabtvs`, `ldrbmi ... !`) - they are data or Thumb in a region the
symbol table calls arm functions. Worth knowing before spending time "deriving" them.

Conclusion for planning: the small-unit tail is dominated by register-role ties that no source spelling
moves, not by unknown logic. Derive, record, park. The productive work left is in larger units where the
instruction mix itself is still unknown (the lock pair `0x0202ce70`/`0x0202cdc8`, the init sequence
`0x020263e0`), and in the helper trees.

## 2.38 The seven-digit symbol trap cost a match again - and how to stop losing it (2026-11-20)

`func_020263e0` (168 B) is matched. It first failed `reg.py` with `link/compile issue` because six of its
nine callees were named from the disassembly listing:

    udis prints          bl #0x2024b04
    the symbol table has func_02024b04   (addr:0x02024b04)

`0x2024b04` is the same number as `0x02024b04` - seven hex digits instead of eight - so a name built
straight from the listing is missing its leading zero: `func_0204b04`, which exists nowhere. `lint_cand.py`
flagged all six immediately (`6 bad extern name(s)`); the mistake was reading `reg.py`'s one-line
"link/compile issue" and moving on instead of running the lint first.

**Rule: after writing any body, run `python build/lint_cand.py <file>` before `reg.py`, and treat
`link/compile issue` as "wrong symbol name" until proven otherwise.** The repair is mechanical: if
`func_XXXXXXXX`/`data_XXXXXXXX` is not in the tables, try inserting a zero after the prefix.

The body, for the record (note the `do/while` around `func_020263b4` and the `u16` local - a `u32` local
compiles to a different loop):

    void func_020263e0(u32 a0)
    {
        u16 n;
        if (*(volatile u16 *)0x027ffc40 == 2) func_020263cc();
        n = (u16)func_02024b04();
        if (func_02024bd4(n) != 0) { do { func_020263b4(); } while (func_02024bd4(n) != 0); }
        func_020266c0(0); func_020266c0(1); func_020266c0(2); func_020266c0(3);
        func_02024948(0x40000); func_02024894(-1);
        *(volatile u32 *)0x027ffc20 = a0;
        func_02026488(0x10);
        func_01ff840c();
    }

`func_01ff840c` is a real itcm symbol (it resolves correctly); the arm9-vs-itcm split is not the problem,
the digit count is.

## 2.39 The lock pair: structure solved, codegen not (2026-11-20)

`0x0202ce70` (acquire) and `0x0202cdc8` (release) guard the object at `data_0204aec0`. Both are fully
analysed; the layout is `L[0]` = a pointer that gets `*(u32 *)L[0] = 0`, `L[2]` = owner,
`L[3]` = recursion count, `L[5]` = key, `L + 4` = the wait queue handed to `func_020251c0`/`func_0202514c`.
`func_02026350` enters and returns the handle that `func_02026364` consumes; `func_020263cc` yields.

Owner -2 (`0xfffffffe`) is the "released" sentinel, so acquire spins `while (L[2] != 0xfffffffe)` and
release restores it when the count reaches zero. Both bodies are in `walls.txt`, lint-clean with correct
symbol names, and both compile.

What remains is codegen only: an extra pointer copy in the first `L[2]` read (mwcc emits `mov r1,r5 ;
ldr r0,[r1,#8]` where retail has `ldr r1,[r5,#8]`), and the release's nested `if (L[3] != 0)` coming out
with the condition inverted. Two formulations each; parked rather than ground further.

**Reading diffs note:** the first four differences reported for both units were `bl` targets, which are
unrelocated and must be ignored. Only the offset shift *after* them was real. When a diff looks like
"everything moved by one instruction", strip the `bl` lines first and see what is actually left.

## 2.40 Short-circuit guard vs early return - read where the branches actually go (2026-11-20)

`func_ov002_02088b0c` (212 B) cost three attempts, and the first two were wrong for a reason that is easy
to repeat: I read four consecutive conditional branches as four early `return`s. They are not. **Every one
of them targets the same address**, and that address is *past* the call they guard - so they are one
short-circuit `&&` around a single call:

    if (a == 0 && b == -1 && c != 0 && d != 0)
        func_ov002_02088be0();

written as early returns, mwcc emits `popne {r4,r5,r6,lr} ; bxne lr` and the whole tail shifts. Written
as the `&&`, retail's instruction stream is reproduced from offset 0 up to 0x84.

Two more levers from the same unit:
* `if (st == 0 || st == 1)` collapses to `cmp r0,#1 ; pophi` - a range check. The only form that keeps
  retail's two separate compares is the label version: `if (st==0) goto body; if (st!=1) return;`
  (the 2.30 lever again, now with a two-value OR instead of a shared tail).
* the final `if (*(s8 *)(p+0x379) == 0)` must **reload** the field rather than reuse the local `st`;
  retail re-reads it at 0xa4.

What is left is the byte increment at 0x84 - retail pools `0x37a` and does `add r1,r6,r0 ; ldrb ;
add ; strb` through r1 where mwcc folds to `ldrb r0,[r6,#0x37a] ; add ; strb`. That is the same
address-materialisation tie as the ov002 getters (2.36), so the unit is parked with the body recorded
rather than ground further.

## 2.41 Correction: there are no per-unit flags, so the tie class must be reachable (2026-11-20)

I speculated in 2.36 that the small units might have come from a compile with different flags or an
SDK-era compiler. That is **wrong**, and the check is one line:

    grep -c "flags|cflags|cpp_args" config/ATOJ/arm9/delinks.txt   ->  0

`delinks.txt` carries only section ranges and `complete` markers; every unit is compiled with the single
`MWCC_FLAGS` in `build/fastmatch.py` (`-O4,p -inline on,noauto ...`). There is no per-unit override
anywhere in the build. So retail's small units were produced by the same compiler with the same flags as
the 1,434 units that match, and **any remaining difference is a source-shape difference, not a toolchain
one.** That is the more hopeful conclusion and it changes the plan: keep probing shapes rather than
declaring the tail unreachable.

Struct-typed access does not help either (`Obj { u8 pad[0x28]; u32 f28; }`, both `o->f28 &= ~4` and a
load-into-local), which rules out "the original used a typed struct access" as the explanation for
`0x02062b48`. The distinguishing feature of retail's form remains that it keeps the address in a second
register and reuses it for both the load and the store.

Six more single-call wrappers were derived this round (ov002 `0x02081078`, `0x02080ca4`, `0x0206d600`,
`0x020882d4`, `0x02063434`; ov001 `0x0204fb34`). All compile, none match; bodies are in `walls.txt`.

## 2.42 RETAIL WAS BUILT WITH TWO COMPILERS: 1.2/b56 for part of the code (2026-11-21)

This supersedes 2.41's conclusion that the small-unit class is a pure source-shape problem. It is not.
The repo ships **25 mwccarm builds** under `tools/mwccarm/` (1.2: b56, base, sp2, sp2p3, sp3, sp4;
2.0: base, sp1..sp2p4; dsi: 1.1..1.3p1). I swept derived bodies across them with the *identical*
`MWCC_FLAGS`, comparing through the normal masked harness (`build/fm_<tag>.py` are copies of
`build/fastmatch.py` with only the compiler path substituted):

    unit                    b56   sp2
    ov002 0x02062b48         Y     -
    ov002 0x02062b68         Y     -
    ov002 0x0207808c         Y     -
    ov002 0x020781b4         Y     -
    ov002 0x02060ca0         Y     -
    ov002 0x02062b20         Y     -
    ov002 0x0204c7e8         Y     -
    ov004 0x020534cc         Y     -
    ov002 0x02063434         Y     -
    arm9  0x0202c430         -     -     (genuinely different shape, still parked)
    ov002 0x02081078/0x02080ca4/0x0206d600/0x020882d4, ov001 0x0204fb34  -  -

**8 of 13 walls byte-match under 1.2/b56 and none under 1.2/sp2.** These are the exact units I had
recorded as an unreachable "register-role / address-materialisation tie class" in 2.36-2.41. They are
not ties; mwcc 1.2/sp2 simply materialises `base+offset` differently from 1.2/b56, and retail used b56
for this code.

Risk to the existing 1,434 matches is small: on a random sample of 60 installed units, b56 and sp2
disagree on exactly **one** (`func_ov004_0205e274`, sp2 matches, b56 does not). Controls
(`func_020263e0`, `func_02024168`) match under b56, 1.2/base and 1.2/sp2 alike. 2.0/* and dsi/* match
nothing here and break the controls, so the choice is only ever b56 vs sp2.

Games of this era commonly upgraded the SDK mid-project, so a split build is the expected explanation,
not a mystery. Adopting it is a **build-configuration change** (per-file compiler selection in the
build, or a global switch plus re-derivation of the ~1.7% that regress), so it needs the owner's
decision - it is not something to slip in under the standing "never change the compiler" rule.
Regenerate the sweep with `build/sweep/mk.py`; matched bodies are saved in `build/sweep/b56_wins.py`.

Note the corollary for the loop: **any wall that is "structure solved, register roles / address
materialisation only" must be re-tested under b56 before being parked.** That is now step 0 of triage.

## 2.43 Per-file compiler selection is now in the build (2026-11-21)

Adopted after owner approval. `config/ATOJ/mwcc_versions.txt` maps a source path to a version tag; the
mwcc ninja rule now takes the compiler from the edge (`$mwcc_exe`, defaulting to `--mwcc-version`), and
`tools/configure.py` adds a non-default compiler to that edge's implicit deps so a compiler swap
rebuilds the unit. `build/fastmatch.py` grew `compile_text(..., compiler=)` / `compare(..., compiler=)`
so a candidate can be probed with the build's compiler before install; `build/fm_<tag>.py` remain the
quick per-version harness copies used for sweeping.

Nine units installed on the first pass, all confirmed by the ROM hash (not just the object compare):

    ov002 0x02062b48 0x02062b68 0x0207808c 0x020781b4 0x02060ca0 0x02062b20 0x0204c7e8 0x02063434
    ov004 0x020534cc

That is 1,434 -> 1,443 functions and 98,436 -> 98,816 bytes, `ninja sha1` OK, `t10 --check` ok.
Everything previously parked in the "register-role / address-materialisation tie" bucket is suspect and
should be re-probed under b56 before any further derivation effort.

## 2.44 Two compilers does not weaken byte-exactness (2026-11-21)

Recorded because it is the obvious objection to 2.42/2.43. Mixing compiler builds per unit cannot
produce an approximate decompilation, for a structural reason: the only pass condition in this project
is the digest of the *linked* ROM (`ninja sha1` against `config/ATOJ/tennis_ATOJ.sha1`). Compiler
version affects one unit's object bytes and nothing else - link order, LCF placement and alignment are
untouched - so a unit compiled with the wrong build shifts bytes and fails the hash. The per-unit map
in `config/ATOJ/mwcc_versions.txt` is therefore evidence about how retail was built, not a relaxation:
it is self-policing, and every one of the nine b56 units was installed through the ROM-hash verifier,
not the object compare.

It is still the original toolchain: `1.2/b56` is a real shipped CodeWarrior ARM 1.2 build in the same
family as `1.2/sp2`, not a substitute compiler. The divergence is narrow - 59 of 60 sampled matches are
byte-identical under both - which is what a mid-project SDK update looks like.

The genuine caveat is toolchain reproducibility, not ROM equality: `tools/mwccarm/` is gitignored, and
`ninja download_tools` fetches one archive from the mirror. Both `1.2/b56` and `1.2/sp2` must be present
for the tree to configure; whether the mirror archive ships b56 has **not** been verified. Documented
in `docs/SETUP.md` under "Per-unit compiler versions".

## 2.45 BIOS SWI stubs: object compare says MATCH, the ROM says no (2026-11-21)

Chasing the "use dsd sig / stop hand-matching SDK code" advice: `dsd sig list` knows only two
signatures (`FS_LoadOverlay`, `FS_UnloadOverlay`), so signature matching has essentially no yield in
this ROM. But the SDK side is already named: 21 non-`func_` symbols in `symbols.txt`, of which nine
(`SoftReset`, `WaitByLoop`, `HuffUnCompReadByCallback`, `Div`, `IntrWait`,
`LZ77UnCompReadNormalWrite8bit`, `RLUnCompReadByCallbackWrite16bit`, `BitUnPack`,
`AutoloadCallback`) were already matched. Their accepted shape is the project precedent for BIOS calls:

    #include "types.h"

    #pragma thumb on
    void BitUnPack(void)
    {
        asm { swi 0x10 }
    }

The ten remaining ones decode cleanly as THUMB (capstone, CS_MODE_THUMB):

    Halt                                svc #6      Sqrt            svc #0xd
    RLUnCompReadNormalWrite8bit         svc #0x14   IsDebugger      svc #0xf
    LZ77UnCompReadByCallbackWrite16bit  svc #0x12   CpuSet          svc #0xb
    GetCRC16                            svc #0xe    CpuFastSet      svc #0xc
    VBlankIntrWait   movs r2,#0; svc #5            Mod   svc #9; adds r0,r1,#0

All ten reproduce **byte-exactly at object level** in that style (for `Mod` the `svc #9` must be
followed by `mov r0, r1`, which mwcc encodes as `adds r0, r1, #0`). **None of them installs**: `reg.py`
returns False, and a manual delinks entry plus `ninja delink` changes the ROM digest
(`ca26365219e43669ea6ee8df59a33927b73812b1`). So this is not a stale-gap-label problem and re-delinking
does not fix it - carving a stub out of the surrounding blob shifts the bytes that follow it. The tree
was restored and re-verified (`sha1` OK, `t10 --check arm9` ok).

Triage rule this establishes: if a function's address has **no explicit entry in `delinks.txt`**, it
lives inside a synthesized gap blob, and an object-level MATCH there is not actionable - only the ROM
hash decides. Splitting a gap means changing how the module is delinked, which is a config change
outside a byte-match batch.

Worth keeping in view for the rest of this class: these are 4- and 6-byte functions, so the entire
remaining BIOS stub set is under 50 bytes. Not worth a config fight.

## 2.46 b56 units cluster by address, and the cluster predicts the next walls (2026-11-21)

Retail compiled one original translation unit with one build, so neighbouring functions share a version.
Acting on that, the three walls sitting next to installed b56 units all flipped:

    ov002 0x020633e0  84B  b56 MATCH (sp2 no)   ORR 0x1000000 / BIC 0x2000000 behind a 0x2000000 test
    ov002 0x02063820  84B  b56 MATCH (sp2 no)   same shape, 0x4000000 / 0x8000000 behind a 0x8000000 test
    ov002 0x0204c824  96B  b56 MATCH (sp2 no)   stride-0x1c slot, clear bit 0x400 at +0x18, then call
                                                func_ov002_0204f3cc(table[a1*0x30], a0 << 5, 0x20)

Only `0x02063c5c` (the third member of the 0x02063820 family) still resists under both builds, so the
family is not uniform - probe it separately rather than assuming the sibling's shape. This took the
session to 12 b56 units; totals 1,446 functions / 99,080 bytes.

Two loop changes came out of it:

* `build/reg.py` now takes an optional trailing version (`... file.c 1.2/b56`). It records the override
  in `config/ATOJ/mwcc_versions.txt` and re-runs `configure.py` *before* `try_unit`, because try_unit
  links the ROM with whatever the build graph says - installing a b56 body while the map still says
  sp2 fails the digest for the wrong reason. This is the bug that made the first three installs report
  False despite an object-level MATCH.
* `build/b56_regions.txt` lists every unmatched function within +-0x400 of an installed b56 unit
  (30 candidates after this batch). That is the probe order for the next pass: read the neighbour's
  version, then derive.

## 2.47 Walking the b56 queue: 1 of 5, and why the other four resist (2026-11-21)

First pass down `build/b56_regions.txt`, smallest entries first:

* **ov002 0x020780b0 (72B) MATCH under b56** - `p = B + 0x1e6c; *p |= 2; p[0x58]=0; p[0x59]=0xa;
  p[0x5a]=0; *p &= ~4;`. Installed; totals 1,447 / 99,152.
* ov002 0x0206308c (56B): retail loads **two** pool constants (`0x027c0000` and `0x3ff8`) and uses
  `ldr r0,[r1,r0]!`; every spelling I tried makes mwcc fold the sum into one address. Same
  address-materialisation family as before, just with a register addend. Parked.
* ov002 0x020638f4 (88B): retail copies a 3-word table (`ldm`/`stm` through the stack) from
  `data_ov002_020fdc30`, then indexes it with a byte from the slot. Neither `u32 v[3]` assignment nor
  a direct `((const u32*)table)[byte]` emits the stack copy; both land far off. Parked.
* ov002 0x020630f4 (116B): starts with a **dead pool load** of `func_02005418` into `r1` that nothing
  uses, plus a discarded `ldrh` of `0x04000208` before the zero store. C cannot be talked into emitting
  dead loads, so this one is only reachable if the original wrote something like an unused
  `(u32)func_02005418;` statement. Parked with the reasoning noted.
* ov002 0x02078208 (124B): logic is clear (set bit 0 on `B+0x1e6c`, three byte stores, then
  `func_ov002_0204f3cc(table[slot[1]*0x30] + 0x20, 0xe0, 0x20)`) but the emitted code lands 120 of 124
  bytes off, i.e. the whole body shifts - the `a0 * 0x158` slot pointer and the byte-indexed table read
  both materialise differently. Parked for a shaped retry rather than a blind one.

Lesson for the queue: b56 grandfathers in the *address-materialisation* walls only when the source shape
is otherwise right. The version fixes register-role ties (2.46), not dead code or table copies.

## 2.48 Two mid-size b56 entries decoded exactly, blocked on instruction order (2026-11-21)

Both bodies below are *semantically* the retail function, and the instruction **kinds** line up - the
remaining difference is pure scheduling (same opcodes, different slots: retail interleaves the
`(a1+1)/2` arithmetic between the `mla` chain, mwcc emits it in a block). Two declaration orders each,
no movement, so they are parked rather than ground further.

    ov002 0x0204c884 (148B)
      idx = ((s32)a1 + 1) / 2;  q = (u32 *)(B + 0x274 + a0 * 0x1c + 0x18);
      *q |= 0x400;
      if (idx == 7 || idx == 0x29)  -> table data_ov002_02100a98[a1 * 0x30]
      else                          -> table data_ov002_02100a94[a1 * 0x30]
      both then: func_ov002_0204f3cc(word, a0 << 5, 0x20)

    ov002 0x02063bb8 (164B)
      v = func_02034148(*(u32 *)(B + a0 * 0x158 + 0x2ec));
      limit = (v >= 0x600000) ? 0
            : ((*(u32 *)(B + 0x1824 + 0xb8) & 0x40000000) ? 0x40 : 0x10);
      if (*(u32 *)(B + 0x1824 + 0xb8) & 0x8000000) limit = 0;
      if (func_ov002_02062dbc(0x100) < limit) *(u32 *)(B + 0x1744 + a0 * 0x38 + 0x24) |= 0x800000;
      else                                    *(u32 *)(B + 0x1744 + a0 * 0x38 + 0x24) &= ~0x800000;

Useful negative result: `diff_summary` counts differ by *more* than the function size here (164 for a
148B unit), which means the compared `.text` is longer than the symbol - always read the count against
the size before treating a wall as "close".

## 2.49 Statement order is NOT the scheduling axis; two structural levers that are (2026-11-21)

Built `build/permute.py` (marks a line with `// P`, permutes every ordering, scores by `diff_summary`,
prints the best, installs nothing):

    python build/permute.py <mod> <addr> <size> <file.c> [version] [--top N]

It answered the question it was built for, negatively: **all 6 orders of 0x0204c884's declarations and
all 120 orders of 0x02063bb8's scored identically.** For these bodies mwcc ignores declaration order
entirely, so "scheduling residue" was the wrong diagnosis - my earlier conclusion in 2.48 was wrong.

What actually fixed `0x02063bb8` (164B, b56, installed) was **signedness**:

    s32 limit;                                    // u32 here emitted addhs/bichs
    if (func_ov002_02062dbc(0x100) < limit) ...    // retail: addge/bicge

and writing the small branch out of line (`if (v < 0x600000) { ... } else limit = 0;` instead of
`if (v >= 0x600000) limit = 0; else { ... }`) so the zero lands in a shared block after a `b`.

What fixed `0x0204c884`'s size (148B exact, from 164B) was **not duplicating the call tail**: retail has
one call site with the table load selected by a result variable,

    if (idx == 7 || idx == 0x29) word = *(u32 *)(a98 + a1 * 0x30);
    else                         word = *(u32 *)(a94 + a1 * 0x30);
    func_ov002_0204f3cc(word, a0 << 5, 0x20);

whereas calling the function in both branches emits the tail twice. It now differs only in register
roles (retail keeps the field address in `lr` and the value in `ip`; mwcc swaps them), so it is parked
as a genuine allocation tie - and it is worth re-reading 2.48's "opcodes match" claim with suspicion:
a diff count above the symbol size means the object is longer, i.e. duplicated code, not reordering.

## 2.50 Twins found the sibling, and the family shares shape but NOT constants (2026-11-21)

`build/harvest.py` returned two overlay units (ov005 `0x02056820`, ov009 `0x0204e650`, both 232B) and
`build/twins.py` found one unmatched twin of an installed unit: `func_ov002_020633e0` (matched, b56)
-> `func_ov002_02063c5c`. All three installed.

The twin is the interesting one, because I had already failed on it twice by assuming it shared its
neighbour `0x02063820`'s constants. Its own disassembly says otherwise:

    0x02063820: test 0x8000000, ORR 0x4000000, BIC 0x8000000   (ORR first)
    0x02063c5c: test 0x00080000, BIC 0x00080000, ORR 0x00040000 (BIC first)

So the two ops also swap order - the 020633e0/02063820 pattern is "set one bit, clear another", and the
sibling does "clear then set". Reading the twin's bytes instead of its family's made it a one-shot match.

Rule for `twins.py` output: take the **shape** from the matched unit, always take the **values and
statement order** from the unmatched unit's own disassembly.

## 2.51 Cross-module twins: a small but real seam, and why it is small (2026-11-21)

`build/crosstwin.py` hashes each function's instruction *shape* (mnemonics with every immediate
masked to `#` and every register collapsed to `R`) across all twelve modules and reports groups that
have both a matched and an unmatched member. Over everything <= 0x200 bytes it finds only **4 groups /
6 open functions** - the overlays are less duplicated than their sizes suggest.

Two were real and both installed:

    ov003 0x02052100 (matched) -> ov005 0x0204f2e0 (96B)   identical body, no module-local symbols
    ov005 0x02051b90 (matched) -> ov003 0x020556b8 (140B)  identical body incl. all constants

Both matched under **sp2 and b56 alike**, which is worth noting: not every family is a version family.

The other four open members are the BIOS SWI stubs, already known to be uninstallable (2.45).

Practical rule: a cross-module twin is a free unit only when the matched body references *no* module
symbols (`data_ovNNN_*` / `func_ovNNN_*`). If it does, the body has to be respelled with the target
module's names, and then it is twins.py work, not a copy. This makes crosstwin cheap to re-run after
every batch (it takes one pass over the symbols files) but not a large source of units.

## 2.52 b56 code exists in arm9 too, so the region map must cover every module (2026-11-21)

Switching from discovery (all three generators drained) back to hand decompilation, the four smallest
fresh targets gave two matches, and **both were arm9 units that only match under 1.2/b56**:

    arm9 0x02018318 (24B) -> s32 f(u8 *p) { u32 *q = (u32 *)(p + 0x24);
                                           *q = *(u32 *)(p + 0x18); q[2] = 0; return 0; }
    arm9 0x0201f980 (28B) -> void f(u8 **a0) { u8 *p = *a0;
                                               if (p) { u32 *q = (u32 *)(p + 0x108); *q |= 4; } }

That corrects an assumption baked into 2.46/2.47: I had built `b56_regions.txt` from overlay anchors
only, because the first twelve b56 units happened to be ov002/ov004. The map now scans **every module**,
and arm9 contributes its own neighbourhoods. Always regenerate from the version map, never from where
the previous wins were found.

Two of the four resisted and are parked: `0x02026a88` (28B) is a 36-byte ldm/stm copy that neither a
3x-word loop, a byte loop, nor a 9-word struct assignment reproduces (52B/36B/28B, no ldm/stm at all),
and `0x0202de8c` (28B) is `ldrh`-test-clear at `0x027fff96` where every spelling differs in all 28
bytes, i.e. the literal is materialised differently rather than the logic being wrong.

## 2.53 The ldm/stm copy idiom is unreachable from C in mwcc 1.2 (2026-11-21)

Three arm9 functions (`0x02026a88` 28B, `0x02026aa4` 36B, `0x0202474c` 36B) are nothing but
N repetitions of

    ldm r0!, {r2, r3, ip}
    stm r1!, {r2, r3, ip}        (0x0202474c: no writeback on r1 at all)

i.e. 12-byte chunk copies, 3 or 4 of them. Every plausible source spelling was tried:

| spelling | result |
|---|---|
| `for (i = 0; i < n; i++) dst[i] = src[i]` (u32 words) | 60B, loop not unrolled |
| `typedef struct { u32 a,b,c; } T;` loop over T | 60B, loop not unrolled |
| unrolled `dst[0] = src[0]; ...` | 68B / 84B, ldr+str per word |
| `*dst = src[i]` unrolled (no dst writeback) | 56B, still ldr+str |
| `memcpy(dst, src, 36)` | 28B, an actual `bl memcpy` |

mwcc 1.2 emits `ldm`/`stm` for aggregate copies only in shapes it never produced here, so the
original construct (a builtin, a pragma-driven copy, or a type mwcc treats as a block move) stays
unknown. Three mechanisms, zero matches: parked, with the sizes recorded above so a future attempt
starts from what was already excluded rather than repeating the loop.

The same pass did yield three units, all b56 and all in the `0x02015000`/`0x02027000` area:

    arm9 0x02027a14 (32B) -> data_0204ab00[a0 * 12 + 8]++
    arm9 0x02015068 (36B) -> *(u16 *)(*(u32 *)(data_02042a68 + 0x14) + 0x1e) |= 1 << a0
    arm9 0x02015040 (40B) -> same, &= ~(1 << a0)

The bit set/clear pair is a useful sanity check on `1 << a0` spelling: `|` produces `orr r0, r1, r2, lsl r0`
and `&~` produces `mvn r0, r2, lsl r0` then `and`, both of which mwcc emits only when the shift is
applied to the *mask*, not the value.

## 2.54 Two more b56 units, and the argument-block push (2026-11-21)

    ov002 0x02085ff8 (40B) -> *(u8 *)(data_ov002_02102070 + 0x47d4) &= ~a0
    arm9  0x020135cc (48B) -> q = *((u8 **)(*(u32 *)(data_02042a68 + 0x14) + 0x104));
                              q[1] = a0; *(u16 *)(q + 4) = a1; *(u16 *)(q + 2) = func_02017360();

The second one repeats the lesson from 2.51 in a different place: retail computes `u8 *r = q + 2`
and stores through `[r]` and `[r, #2]`, while `*(u16 *)(q + 4) = ...; *(u16 *)(q + 2) = ...` lets
mwcc fold both stores onto the single base `q` and comes out four bytes short. When two stores differ
by a field offset, give the inner offset its own pointer variable; the fold is otherwise automatic and
invisible in the sizes of the surrounding code.

`0x02024b5c` shows a fold of a different kind: `*p |= 0x80000000 >> a0` compiles to a single
`orr r2, r2, r1, lsr r0`, while retail keeps the shift in a separate instruction. A temp variable does
not stop it, and a volatile mask balloons the function to 64B. Retail also picks PL/MI where the
obvious signed source gives GE/LT, so the condition spelling is wrong too - a genuinely different
source, not just a missing barrier. Parked.

`0x0203b4e4` has retail saving its arguments with `push {r0, r1, r2, ip, lr}` rather than allocating
callee-saved registers, and restoring them before the `bic`/`eor`. Both a no-argument and a
four-argument callee declaration land on r4/r5 with a frame, so this is an allocation decision made
before the source can express it. Parked.

## 2.55 Signedness and the address-of-struct local (2026-11-21)

    arm9  0x020279e0 (52B) -> data_0204ab00[a0*12] = a1; [+4] = a2; ([+8])++; return [+8];
    ov002 0x02084210 (52B) -> data_ov002_02102070[0x36ac + 0..4] = 0 (five byte stores)
    arm9  0x02015524 (52B) -> counter at *(*(data_02042a68 + 0x14) + 0xc), ++ then threshold

`0x02015524` is the cleanest signedness demonstration yet. The same body written with `u32` for the
counter emits `movhs`/`movlo` against `cmp r0, #0x12c`; switch the field to `s32` and it becomes
`movge`/`movlt`, which is what retail has. The *size* is identical - only the condition field of the
two predicated `mov`s changes - so byte compare catches it, but a mnemonic-only diff does not show a
length problem and it is easy to read as a scheduling difference.

It also needed the inner pointer split out (`s32 *q = (s32 *)(p + 0xc); *q = *q + 1;`) while the
second read re-derives the whole chain from the global, which is why retail reloads
`data_02042a68 + 0x14` instead of reusing the first load.

Struct-by-value returns remain the weak spot: `0x0203684c` (40B) and `0x02032b00` (48B) both want the
argument copied to the stack with `add r0, sp, #0` and then accessed through a `u8 *` so that mwcc's
unaligned-load idiom appears for the byte at +7. Every spelling tried either kept the struct in
registers (36B / 20B) or spilled it differently (44B). Retail is memory-resident where mwcc insists on
registers; parked.

## 2.56 Field-order walls and why "identical twins" are not automatic wins (2026-11-21)

    arm9 0x0201c9e0 (56B) -> func_0201be3c(a0->0x40); func_0201779c(data_02047dcc, a0);
                             *(u32 *)(a0 + 0x24) &= ~1;
    arm9 0x020341f4 (56B) -> a field-to-field copy chain plus a subtract of an AND product

`0x020341f4` is worth keeping as a template: retail's order is

    [0x24] = [0x1c]; [0x28] = [0x20]; then *p -= ([0x18] & [0x2c]); then [0x34] = [0x18]

with the `ldr` of `[0x18]` *before* `[0x2c]` even though the AND reads them left to right. Writing the
statements in exactly retail's order, with a pointer variable for the `0x28` slot, matched first try
under b56. Statement order matters when the fields alias.

`0x020b6b60` and `0x020b732c` are byte-identical twins (52B, six `strh` zeroing a stack buffer) and
still would not match: retail is doing something with three separate base pointers where the zero is
materialised *after* the first `add`, and every array-initialiser or three-pointer spelling I tried let
mwcc fold the first store to `[sp]` and come out 4-8 bytes short. Identical siblings are only free
units once one member matches; two twins that both resist cost twice the budget for the same shape.

Also parked: `0x02028920`/`0x02028958` (call with the pre-increment sum; the base register choice is
r0 in retail, r3 with a frame in every spelling), `0x0201fd50` (five I/O stores; the three-base pooling
pattern is not reachable from per-address casts), `0x02032c38` (retail hoists `n++/p++` above the test
and reads the byte through the unaligned idiom; mwcc makes the load the loop condition) and
`0x0203b698` (table walker whose rotation puts the `cmp p,#0` at the bottom).

## 2.57 A sibling can differ in reload policy, not source (2026-11-21)

    ov002 0x020636b4 (108B) -> q = data_ov002_02102070 + 0x1744 + a0 * 0x38;
                               if (*(u32 *)(base + 0x18dc) & 0x40000000)
                                   if (func_ov002_02062dbc(0xa) == 0) q[9] |= 0x10000000;
                                   else                              q[9] &= ~0x10000000;

Straight shape: a `data_ov002_02102070 + k` base, a `* 0x38` element stride (the `mla`), a guard
against a bit in another global, and a set/clear pair driven by a call result. Matched first try under
b56.

Its sibling `0x02063874` (128B) has the same skeleton with different constants (`== 2` guard,
`0x8000000` bit, `0x100` argument, `< 8` threshold) and would not match at any of three spellings.
The reason is visible in the disassembly: retail writes `add r4, #0x24` *before* each of the three RMW
sites, while mwcc folds the slot into the store as `[r4, #0x24]`. In the matched sibling the same
`q[9]` spelling *did* produce a separate `add`. So the two functions were compiled with different
reload/re-materialisation behaviour for what looks like identical source structure - which means one
of the two sources differs in a way the instruction pattern does not reveal (most likely a macro or an
inline accessor that expands differently at one site).

Practical consequence: matching a sibling does not guarantee the next one, even when the instruction
shapes agree; treat each as its own unit once a spelling fails twice.

## 2.58 Keeping a global in a register, and the arena pair (2026-11-21)

    arm9 0x02018018 (136B) -> sibling of 0x0201842c: func_020179c8(a0, 0x45585048, q + 0x14, a1, a2),
                              two u16 clears, a `&= ~1`, a two-word stack pair fed from a0 + 0x18 and
                              a0 + 0x1c, func_020180a0(pair, 0x4652), then four stores into q
    arm9 0x020186a0 (132B) -> queue insert: guard call, slot from a second call, three fields stored
                              in order a0, a2, a1, a3, third call, then `*(g + 0x10) += slot[3]`

`0x020186a0` is the clearest example yet of a global that has to be cached by hand. My first version
passed `data_02045650` directly at each of the three call sites; mwcc then emitted three separate
pc-relative loads and allocated only six callee-saved registers (120B, no `sb`). Assigning
`u8 *g = data_02045650;` at the top and passing `g` matched immediately: retail keeps the address in
r5 for the whole function and frees `sb` for the first argument. So when retail shows a pooled global
loaded once at the top and reused across calls, the source has a local pointer, not repeated references.

Also note the field store order in the same unit: retail stores a0, a2, a1, a3 into slots 0..3 — the
second and third arguments are swapped relative to declaration order. Writing `p[1] = a2; p[2] = a1;`
reproduces it; writing them in argument order does not.

The arena pair `0x02018330` / `0x020183ac` (124B, 128B) is understood at the source level but not
matched. Shape: `newptr = ~(align-1) & (end - size)` for the bump-down member and
`~(align-1) & (base + align - 1)` for the bump-up one, guard against the opposite field, `u16` flags
test at `p[-1]` (bit 0), call `func_020048e8(0, ptr, size)`, update the field, return the pointer.
Three spellings of the flags test identified the exact form (word load, `& 0xff`, u16 widening, `& 1`,
not `ldrh`), but retail additionally stores the zero constant to `[sp]` and reloads it for the call -
the only memory-resident-partial I have seen in this codebase - which no spelling reproduced. Best
copies: 112B and 116B against 124B, 108B against 128B. Parked with the body shape written down.

## 2.59 Bitfields, and a note on the helper trees (2026-11-21)

Two idioms worth remembering from `0x0201f6c0` (172B, parked at exact size):

**A 1-bit bitfield test is not `tst`.** `ldr r0, [p, #0x108] / lsl r0, r0, #0x1f / asrs r0, r0, #0x1f /
beq` is mwcc extracting a bitfield member, and the `asr` (not `lsr`) says the member is declared
**signed** (`s32 b : 1`). With `u32 b : 1` the same source emits `lsrs` and the unit never matches.
The write side then comes out as `bic #1 / orr #1`, which is the giveaway that the read and the write
are the same bit of the same field.

**Compute the pointer after the guard call.** Retail keeps `a1` in a callee-saved register across
`func_0201f8c8()` because the element address `data_02048760 + a1 * 0x144` is only formed afterwards.
Declaring that pointer before the `if` lets mwcc fold the `mla` into the prologue and drops a register
(144B). This is the same lesson as 2.58 (a value that is live across a call has to be written as one).

That unit still will not match: retail permutes the saved registers and re-materialises the `+0x108`
address at the bitfield write, while every spelling keeps it in a register or folds it. Exact size,
identical instruction count, different allocation - the same wall class as 2.46.

**Helper trees.** The scout and opencode workers are paused indefinitely for usage reasons, so the
per-batch `cp` of `notes/`, `build/walls.txt` and the generators to `/m/ATOJ-scout`,
`/m/ATOJ-worker` and `/m/ATOJ-delta` is suspended until further notice. All work stays in this tree;
nothing in the release procedure changes apart from dropping that copy step.
