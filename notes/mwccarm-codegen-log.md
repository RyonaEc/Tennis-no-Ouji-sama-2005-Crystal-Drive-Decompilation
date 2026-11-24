# mwccarm codegen evidence log (ATOJ)

Dated record behind `notes/mwccarm-codegen.md`. The playbook has the lever
table; this file has the measurements, the bodies that were tried, the exact
diffs and the verdicts. Section numbers here (`2.60`, `9.1`, ...) are the ones
quoted by `tools/match/walls.txt`, commit messages and the playbook.

Later entries supersede earlier ones when they contradict: read the date, and
paths below are as they were written - the matching tools have since moved from
`build/` to `tools/match/`.
treat the newest statement of a rule as the current one.

---

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
python tools/match/t10.py --try <name>            # try_unit(...) from Python

# gates, in order
python -m ninja sha1                        # whole-ROM digest
python tools/match/t10.py --check arm9 ov002 ...  # delink link-state sanity
python -m ninja report                      # progress numbers
```

`tools/match/cdiff.py <mod> <name>=<file.c>` compiles one file and prints a
reloc-aware instruction diff against retail without touching the tree, which is
the fastest way to read a residue. `tools/match/auto.py` exposes `set_module`, `FUNCS`
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
batching), `fuddlesworth/pokebw2` `pokebw2/docs/matching.md` (narrow parameters change
caller scheduling; direct-global accesses instead of cached; pointer-reference
count flips added-vs-folded), `ReGame-Labs/dwds_decomp` (1.2/sp4 `-O4,p`),
`7ucina/acww-decomp` (1.2/base, Thumb; renamed from `6elphegor/`),
`InusualZ/mhtri-decomp` (a Wii decompilation - subscript vs pointer-sum operand
order), and the `decomp.wtf` MWCC guide.

A dated survey of the whole field - which projects are actually source-complete,
and which document answers which problem - is in [`ds-prior-art.md`](ds-prior-art.md).

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

This corrects an earlier note in `tools/match/walls.txt` that called `func_02039630` (912 B) a "generated
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
   **call that was inlined**, which matches the two units in `tools/match/walls.txt` that carry one
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

`tools/match/ready.py <mod> --frameless` plus a scan for "at least 3 branches to one target and at least 2
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

`tools/match/jt.py` walks every ready unit (`auto.FUNCS` minus `src/<mod>/*.c`) and reports each one whose
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

I tried to automate the pure-map class end to end (`tools/match/jtgen.py`): find the `addls pc, pc`, read the
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

`tools/match/jtgen.py` is kept because it is correct and cheap; when a genuine map turns up it produces the
source in one call. `tools/match/jtrun.py` runs it across `tools/match/jt.txt` and reports how many units it could
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
establish whether the shape is reachable is enough; record it in `tools/match/walls.txt` and move on. This
is the same category as the register-allocation ties in the main notes, and it is not a sign that the
source is wrong - mwcc at this version simply does not materialise here.

## 2.15 A census of what is actually left (2026-11-20)

`tools/match/census.py` filters the ready pool down to units that can plausibly be matched as units at all:
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

`tools/match/famsurvey.py ov002` reports `13x756 val 9828 0x0208f860 0x0209a5f4 0x0209c728 0x0209eb18
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

`tools/match/lint_cand.py` now checks every `extern` name in a candidate against all symbol tables
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

`python tools/match/autorun.py 45 120 arm9` reports `already tried 465`, `bailed 92`, `testing 0`,
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
missing zero) which `tools/match/lint_cand.py` flagged before install.

Running total from harvests: **28 units, 3,980 bytes** that were sitting finished in the helper trees
and never claimed. The loop is `harvest.py` -> `lint_cand.py` -> `install_helpers.py` (or `reg.py` one at
a time) at the start of every session; it takes a minute and has paid every time it has been run.

## 2.28 The harvest was blind to every overlay (2026-11-20)

Root cause of three sessions of "the helper trees are drained": `build/harvest.py` and
`build/install_helpers.py` resolved candidate sizes through `tools/match/auto.py`'s `FUNCS`, and that table
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

All three families are in `tools/match/walls.txt` with their full retail sequences and the closest body, so
the helper trees can pick them up with the derivation already done.

## 2.32 Matched-twins: mine the units that are already done (2026-11-20)

Twenty-odd turns were spent deriving bodies from scratch for functions that had a **twin already in the
tree**. `tools/match/twins.py` fixes the target selection: it groups every function in every module by
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

**Rule: after writing any body, run `python tools/match/lint_cand.py <file>` before `reg.py`, and treat
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
`MWCC_FLAGS` in `tools/match/fastmatch.py` (`-O4,p -inline on,noauto ...`). There is no per-unit override
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
`tools/match/fastmatch.py` with only the compiler path substituted):

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
rebuilds the unit. `tools/match/fastmatch.py` grew `compile_text(..., compiler=)` / `compare(..., compiler=)`
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

Toolchain reproducibility is not a caveat: `tools/mwccarm/` is committed, so both `1.2/b56` and
`1.2/sp2` are present in a clean checkout and the tree configures without a download. Documented in
`docs/SETUP.md` under "Per-unit compiler versions".

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

* `tools/match/reg.py` now takes an optional trailing version (`... file.c 1.2/b56`). It records the override
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

Built `tools/match/permute.py` (marks a line with `// P`, permutes every ordering, scores by `diff_summary`,
prints the best, installs nothing):

    python tools/match/permute.py <mod> <addr> <size> <file.c> [version] [--top N]

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
`tools/match/twins.py` found one unmatched twin of an installed unit: `func_ov002_020633e0` (matched, b56)
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

`tools/match/crosstwin.py` hashes each function's instruction *shape* (mnemonics with every immediate
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

**Helper trees (removed).** The `ATOJ-scout`, `ATOJ-worker` and `ATOJ-delta` worktrees were deleted
after confirming that each one's commits were ancestors of `main`; their uncommitted notes and
rosters were identical to this tree's. Their briefs (`notes/scout-brief.md`,
`notes/agent-protocol.md`) and the scripts that existed only to copy work between trees
(`build/inbox.py`, `build/install_helpers.py`, `build/harvest.py`) were removed with them.
Everything happens in this tree now. `AGENTS.md`, `CONTRIBUTING.md`, `notes/mwcc-matching-levers.md`,
`notes/ghidra-headless.md` and `notes/objdiff-vs-digest.md` stay for outside contributors and other
agents.

## 2.60 Five exact-size near-misses in one session (2026-11-21)

The discovery tools were re-run now that more units are matched. `twins.py` reports nothing (every
same-skeleton pair has one matched member already); `crosstwin.py` finds only the BIOS SWI stubs; but
`famsurvey.py` turned up five *unmatched* identical-skeleton pairs, which is where the remaining cheap
work is. Two of them are now understood to the instruction and blocked on register naming alone:

    0x020208b0 / 0x0202093c (140B x2)   set/clear of a media bit, 5 constant differences
    0x02023df8 / 0x02023e58 (96B x2)    two-way dispatch on a stored handle
    0x0202d158 / 0x0202d22c (212B x2)   not yet decoded
    0x02033bd0 / 0x02033c1c (60B x2)    APSR I/T-bit toggles - mrs/msr, needs asm, skip
    0x02028920 / 0x02028958 (56B x2)    parked earlier (2.56)

Three findings worth keeping:

**Branch-vs-if-conversion depends on nesting, not on the condition.** For `0x020208b0` the source
`if (a0 == 0) { clear } else if (a0 == 0x100) { set }` is if-converted into `ldreq/biceq/streq`
(136B). Writing it inverted - `if (a0 != 0) { if (a0 == 0x100) { set } } else { clear }` - produces
retail's actual branch to the clear block and exactly 140B. So when retail *branches* to a small block,
nest the condition instead of writing the small block first.

**The register a scratch value lands in follows load order.** In both 140B twins retail computes `~a0`
into ip *first* and then loads the two u16 fields into r2/r3; mwcc here computes `~a0` into r3 and
loads into ip/r2. Five spellings (separate temps, a `mask` variable, a slot pointer, a fully inline
expression, declaration-order changes) all land on the same wrong allocation. This is the 2.46 wall
class; exact size is not close enough to commit.

**`famsurvey` is the right tool now.** The scatter-gun over 90 small targets yielded one match per six
attempts; the same effort spent on a 2-member skeleton group is worth 280-424 bytes. Future batches
should start from `famsurvey.py` output and only fall back to `fresh.txt` when the groups are exhausted.

## 2.61 The 212B famsurvey pair decoded (2026-11-21)

`0x0202d158` / `0x0202d22c` are not constant-twin variants: they are two *different* state setters that
share a skeleton (same prologue, same spin-wait, same five field stores) and differ in the order of the
first two stores and in which tail function they install (`func_0202d300` vs `func_0202d3f0`). Both are
6-argument functions, so the 5th/6th args arrive at `[sp+0x20]`/`[sp+0x24]` after the 8-register push;
mixing those up silently swaps which argument selects the tail path.

Three codegen lessons from getting the body from 224B to exactly 212B:

1. A `u32 *flag` local that is live across two calls costs a callee-saved register (`fp`) and a 4-byte
   frame - 224B instead of 212B. Retail recomputes `*(u32 *)(b + 0x34)` from the base each time, so the
   source must not cache it.
2. `if (a5 == 0) { tail } else { return 1; }` compiles to `bne` with the tail out of line. Writing
   `if (a5) { func_0202cf10(...); return 1; }` *first* and the zero path as the fall-through gives
   retail's `beq` to the out-of-line zero block. Same rule as 2.60: retail's condition is the one that
   *jumps away*, so it belongs on the small `if`, not on the `else`.
3. Referring to the global again (`*(u32 *)(data_0204aec0 + 0xd0) = ...`) instead of `b` makes mwcc
   reload the base for that one store, which is what retail does. Using `b` throughout kept the base
   in r6 and lost the load.

Residue is one folded RMW (`|=` never materialises its address here) and one displaced load shape.

## 2.62 fresh.txt was misparsed for three batches (2026-11-21)

The remaining-candidate list `build/fresh.txt` is `mod addr size name`, so the address is field 1, not
0. Two batches of "fresh.txt is exhausted / all 38 candidates are walled" were wrong for that reason:
the script was reading the module name as the address and matching every row against `walls.txt` as
`0x0`. With the parse fixed, arm9 still has 38 untouched small functions (28B-88B), which is where the
two installs this batch came from (`func_020279e0` 52B, `func_0201fc1c` 32B).

Two shapes from this batch worth reusing:

- **`func_0201fc1c`**: `if (s->a < s->b) s->a = s->a + 1;` on a struct with a `u8 pad0[8]` head matches
  only when written with the struct (b56), not with `s32 *p = (s32*)(a0+8)` (28B vs 32B). The struct
  form keeps the `add r1, a0, #8` materialisation that the pointer form folds away.
- **`func_020279e0`**: a 12-byte-stride table increment. `*q = *q + 1; return p[8];` with `q = p + 8`
  and the return reading `p[8]` is what produces retail's `ldrb` reload after the `strb`; using `q` for
  both the store and the load drops the reload and 4 bytes.

## 2.63 b56 swept the 68-76B band (2026-11-21)

Four installs from the untouched tail of `fresh.txt`, and in this band **every** match came from
`1.2/b56`, none from sp2: `func_020289f4` (68B), `func_020298e4` (76B), `func_02028c98` (72B),
`func_020187b0` (72B). The b56 map in `config/ATOJ/mwcc_versions.txt` therefore keeps growing with
names the original toolchain evidently compiled with the b56 build - when a 68-76B candidate is
exactly sized but permuted, test b56 before rewriting the source.

`func_020187b0` also settled the compound-assignment question from 2.61: `*(u16 *)(a0 + 0xc) -= 1;`
works (72B, b56) where `u16 *cnt = (u16 *)(a0 + 0xc); *cnt = *cnt - 1;` costs 12 extra bytes (84B) by
keeping the pointer live across the earlier call. Compound assignment on a fresh computed lvalue lets
mwcc rematerialise the address after the call, which is what retail does.

`func_02028c98` is the same lesson from the other side: the HAL lock pattern
`h = func_02026350(); work; func_02026364(h);` keeps `h` in a callee-saved register without spilling,
and the `&= ~0xf` on a field between two calls must stay a compound assignment on `*(u32 *)(a0 + 0xc)`.

## 2.64 fresh.txt is stale; derive the queue from src, and probe every compiler at once (2026-11-21)

Two process fixes, both cheap, both worth having before the next unit.

**`build/fresh.txt` contains already-matched functions.** Its entries are the
small unmatched set from the census that produced it, but the census is not
re-run when a unit is installed, so names that matched in a later batch stay in
the file. Three of the first five candidates this round (`0x02018318`,
`0x0201f980`, `0x02027a14`) already had `src/arm9/<name>.c` and matched; the
"install" only rewrote their bodies with an equivalent spelling, which is a
cosmetic change to matching code and had to be reverted. The queue to trust is
`tools/match/true_fresh.txt`, regenerated from the live tree: every entry of
`auto.FUNCS` whose `src/<module>/<name>.c` does not exist. Also check the
symbol name, not just the address: `0x02085ff8` is `func_ov002_0202085ff8` in
`symbols.txt` (the nine-digit name trap, 2.38), and passing the eight-digit
form creates a new file and a duplicate delinks entry that fails to link.

**Probe all retail compilers in one call.** `tools/match/vprobe.py <mod> <addr>
<size> <file.c>...` compiles each candidate with `1.2/b56` then `1.2/sp2` and
prints `MATCH` or the first six differing instructions for each, masking
relocations before the compare. This removes the manual "test b56 before
rewriting" step; in the 28-36 B band every hit this round was b56-only, and
sp2 was shorter or differently shaped in every miss.

**The absolute-address trap in a candidate.** Writing `*(u8 **)0x02102070`
instead of referencing `data_ov002_02102070` compiles to the same instruction
bytes - the pool word is just a literal - and `compare()` calls it a match, but
the object then has a raw absolute word and `t10.try_unit` fails to link
(`link/compile issue`). Reference the symbol (`extern u8 *data_...;`) for
anything that is not a plain constant.

Matches: `func_0202de8c` (28 B, b56: `if (*p & 1) *p &= ~1;` on the DTCM
halfword at `0x027fff96`) and `func_ov002_02086020` (36 B, b56: `u8 *p =
data_ov002_02102070 + 0x47d4; *p |= mask;`, the OR sibling of the
`&= ~mask` unit from 2.60).

Parked this round: `0x020263c0` (retail `subs r0,r0,#4 ; bhs`; `while`,
`do/while`, `while ((n -= 4) >= 4)` and `return n` spellings all produce a
separate `cmp` at 16 B); `0x0202612c` (two-call wrapper with no `sub sp,#4`
frame; mwcc always frames it); `0x02005418`, `ov004 0x0204cbdc`,
`ov002 0x0206308c`, `ov008 0x0204e51c` (retail pools `0x027c0000` and
`0x3ff8` as two words and uses `ldr r0,[r1,r0]!`; every spelling folds them to
one literal or loses the writeback); `0x02015068`/`0x02015040` (retail holds
the global in `r1` and the mask in `r2`, mwcc swaps them; four spellings);
`ov003 0x020b6b60`/`0x020b732c` (exact 52 B, `r0`/`r1` roles swapped in the
zero-init sequence); `0x020135cc` (the `data_02042a68 + 0x14` chain folds to
`[r2]` instead of `[r2,#0x14]`).

The ldm/stm copy family got one more shape: a 12-byte struct assignment
(`dst[0..3] = src[0..3]`) produces `stmdb sp!,{lr}` / `sub sp,sp,#4` plus
`ldm/stm` through `lr`/`ip`, 68-84 B, so the guess in the advice list (single
36/48 B struct assignment) does not reach retail's frameless
`ldm r0!`/`stm r1!` pairs.

## 2.69 Two-call wrapper matched; register-name parks (2026-11-21)

`func_0201f170` (arm9, 76 B) matched first try on `1.2/b56`:

    u32 s = func_02026350();
    u32 v = func_02017784(data_02047f08, 0);
    if (v) func_0201779c(data_02047f08, v);
    func_02026364(s);
    return v;

The pattern worth reusing: a "lock/enter, look up, unlock/leave" wrapper keeps the lock token in a
callee-saved register across the whole body and re-references the same global twice rather than caching
its address.

Parks: `0x02081078` (60 B) is instruction-for-instruction identical to the candidate and differs only in
naming the derived pointer r0 rather than r2; `0x02080ca4` (68 B) puts the loop counter in ip where mwcc
picks lr and emits an extra `mov ip, lr` (76 B), with a different split of the `0x3e6` store offset.

## 2.68 BIOS-call wrapper and two more parks (2026-11-21)

`func_020307c8` (arm9, 64 B) is a one-shot wrapper: `u32 v = 0; CpuSet(&v, data_0204c180, 0x05000001);
*(u16 *)((u8 *)data_0204c180 + 2) = func_02024b04();`. The thumb callee (`CpuSet`, `kind:function(thumb)`)
comes out as `blx` on its own, and the local is only there so `CpuSet` has a source address - the 4-byte
frame pair is real here, unlike the `0x020208b0` park. Matched first try on `1.2/b56`.

Parks from the same band:

* `0x0202497c` (76 B) - `p[idx] = a`, `q[idx] = b`, then `third[idx] = func_02024910(1 << (idx + 8)) &
  (1 << (idx + 8))` with the three arrays at `data_02048ccc`, `data_02048cd4`, `data_02048cd0` and a
  `cmp`-style stride of 12. Byte-exact except one schedule: mwcc hoists the `data_02048cd4` address load
  above `add r4, r0, #8` / `mov r5, #1` / `lsl`, retail keeps it after the mask. Three spellings.
* `0x02029f54` (76 B) - `func_0202d5ec(data_0204ac00, q, r, {func_02029fa0, p, 1}); return 6;`. Retail
  writes the three struct words straight to `[sp]`, `[sp+4]`, `[sp+8]` under `sub sp,#0xc` with only `lr`
  saved. Struct-by-value and a compound literal both force `push {r4,r5,lr}` and 100B; six scalar
  arguments collapse the stack block to 60B. The literal copy is what mwcc will not do here.

## 2.67 A 4x196 channel-reset family, and the u32 accumulator trap (2026-11-21)

`famsurvey.py` listed `ov003` 0x02066e84 / 0x020b8904 / 0x020b8e54 / 0x020dbad4 as one 196-byte skeleton.
The body is: read the u16 at `0x04001048`, `bic #0x3f`, `orr #0x1f`, `orr #0x20` (two separate ORs, per the
playbook row), store it back; call `func_02021b14(0x04001050, 0xe, 0x24, *(u16 *)(b + 0x897e),
*(u16 *)(b + 0x8986))` with `b = data_ov003_020eb720`; then the predicated `++` under `< 0x10` and `--` under
`!= 0`; then `*(u32 *)0x04001000 = (word & ~0x1f00) | <tail>`, returning 0. The siblings differ only in that
tail constant (`0x1500` in three, `0x100` in `0x020b8904`).

**Trap:** writing the register value as `u16 v` makes every statement truncate (`lsl r1,r1,#0x10` /
`lsr r1,r1,#0x10`) and the unit lands at 204B. Retail keeps one 32-bit value in a register across
`bic`/`orr`/`orr` and only truncates at the `strh`, so the local must be `u32 v = *(u16 *)0x04001048;`.
That single declaration took the body from 204B to an exact 196B match on `1.2/b56`. All four installed.

Parks from the same sweep: `0x020208b0`/`0x0202093c` (140 B pair - exact at 136 B but retail carries a
4-byte frame pair with no stack access, plus a three-register rotation) and `0x02028920`/`0x02028958`
(56 B on b56 with a load-order and callback-register swap). Both are in `walls.txt` with their residues.

## 2.66 Wall re-probe under b56 (2026-11-21)

`walls.txt` was written when sp2 was assumed universal, so every parked unit carried a
"unreachable under 1.2/sp2" label that stopped being true on the b56 discovery. `tools/match/wallprobe.py`
parses the wall list, skips units that now have a source, finds their stored bodies in `build/cand` by
address, and probes each under `1.2/b56` then `1.2/sp2` with relocations masked.

First run over 190 wall addresses: 28 already installed, 26 without a `FUNCS` entry, 45 with no stored
body, 110 bodies compiled, **2 matches, both b56-only**:

* `func_0201842c` (arm9, 80 B) - copy constructor: `func_020179c8(obj, 0x46524d48, p + 0xc, a, b)` then
  three word copies from `+0x18/+0x1c` and a zero. The park note blamed the `add r4, a0, #0x24` hoist;
  under b56 the same body reproduces it. Installed, `mwcc_versions.txt` entry added.
* `0x02085ff8` (ov002, 40 B) - was already matched, under a source file named
  `func_ov002_0202085ff8.c` (duplicated `020`). Renamed to `func_ov002_02085ff8.c`, delinks entry
  follows, and it is b56 in the version map.

Both were installed through `reg.py` and confirmed by `ninja sha1`. Lesson: a park is a result for the
compiler it was probed with, so re-probe the wall list whenever the version model changes.

## 2.65 Nine small units, and four walls that are pure colour (2026-11-21)

Swept the unwalled 16-84B band with `vprobe` (b56 first). All nine below matched
under `1.2/b56` and needed a map entry.

| Unit | Size | Shape |
|---|---|---|
| `func_ov002_02058998` | 56 | `mla` stride 0x158 into `data+0x2e4`, `if (hp > 0) hp--` at 0x138 |
| `func_ov002_02072324` | 20 | `*(u16 *)(p + 0xa) += 1` |
| `func_ov002_0209e44c` | 72 | `data+0x1824`, six zeroed words 0x5c-0x70, `&= ~0x3f0` at 0xb8 |
| `func_ov002_020ace70` | 72 | byte-identical twin of the above (different unit, same constants) |
| `func_ov003_02089b64` | 64 | `u16` counter from +0x170, early `return 4` on +0x13c, `min` with +0x142 |
| `func_ov002_0205ffe8` | 60 | flag bit 0 at +0x28, `if (< 0x1464b40) ++` at +0x260 |
| `func_ov002_0204f898` | 64 | byte copy into +0x1930 from `data+0x4f21`, zero-test then `++` at +0x18f0 |
| `func_02033dec` | 84 | `if (data_0204c274 == 0) { init(); if (cb) { cb(); cb = 0; } } tail(x)` |
| `func_ov001_0204fb34` | 80 | `func(0xc0, data+0x134c, 0x400)` then `*(u32 *)(data + 0x1c) = 0x7f` |

Two levers worth keeping:

* The 0x4f21 copy is written as a `u8 v` local, stored, then re-read from the
  field (`if (p[0x10c] == 0)`), which is what makes retail reload it instead of
  reusing the register.
* `func_ov001_0204fb34` only matches with `volatile u32 v = 0xc0;`: retail does
  `str r1,[sp]` then `ldr r0,[sp]`, so the constant must be a volatile local to
  survive the expression. A plain literal produced a 72B version.

`func_02033dec` compiled to identical bytes on the first try but failed the
link: the callee is `func_0203b640`, not `func_02033b640`. Same seven-digit trap
as §2.38; the compile succeeded because the name was only a relocation.

Parked after 3-6 spellings each, all in `tools/match/walls.txt` now: `0203b4b0`
(mwcc always adds a 4-byte frame slot), `02036650` (string loop peeling),
`02032be8` (loop entry check orientation), `020202b4` (DISPSTAT update: register
pair and eq/ne blocks both inverted), `02029cc4` (guard colouring), `0207be2c`
(base/q addressing split), `02068c78` (frameless double reload of the module
pointer), `020882d4` (index loop: C hoists the element pointer or spills to
r7/r8).

## 2.70 Small-band sweep: one match, six parks (2026-11-22)

Re-ran the small-band queue after pruning units that already had sources (`true_fresh.txt`
is stale: it still listed several units matched in `f2028a1`). New this pass:

* `func_02018818` (arm9, 72 B) - `if (func_02018ba4(p)) return 0;` then
  `*(u16 *)(p + 0xa) = func_02018bbc(p, *(u16 *)(p + 0xa));` and `++*(u16 *)(p + 0xc)`,
  `return 1`. Matched first try under b56; the argument order of `func_02018bbc`
  (pointer first, field second) is what keeps the `ldrh` in `r1` before the call.

Parked with residues in `walls.txt`: `02024b5c` (`ge/lt` vs retail's `pl/mi` condition
encodings - not reachable from C), `020248c8` (retail's 4-byte frame and dead DISPSTAT
re-read), ov004 `0204d980` (lead-byte test colouring), ov004 `0204e8a4` (`mul` vs `mla`),
ov008 `0204ce20` (pre-indexed reload on the decrement path), ov002 `02052608` (constant
folding of `0x1824+0xb8`).

Also confirmed two `str lr` / `pop {pc}` / `mov pc, lr` stubs (`0203b564`, `0203b578`,
`0203b58c`) and the `mov r0,#k; b body` selector pairs (`02033bc0`, `02033c0c`,
`02033c58`) are hand-written non-interworking assembly: the build's fixed `-interworking`
flag cannot produce those epilogues, and `str lr` is not reachable from an `asm` block.

## 2.71 ov005 2x140 family matched; arm9 2x96 confirmed a wall (2026-11-21)

`func_ov005_02064040` and `func_ov005_0206756c` (140 B each; identical bar the base
pointer constant and one callee) matched on `1.2/b56`. Three findings:

* **Branch polarity.** Retail if-converts the store block (`movhs`/`strhhs` + `bhs`).
  Writing `if (*(u16 *)(p + 0xa6) < 0x10) { calls } else { stores }` - large branch
  first, small branch in the `else` - reproduces it; the natural `>= 0x10` form branches
  and lands 8 bytes short.
* **Counter offset.** The tail must be written from the base pointer, not through the
  `b + 0x4300` pointer: `(*(u16 *)(b + 0x43a6))++` makes b56 pool 0x43a6 and emit
  `ldr r1,[pc] / mov r0,#1 / add r2,r4,r1`, matching retail. Through the pointer, or
  with a `u32 off = 0x43a6` local, it folds to `add #0x4300` + `[r1,#0xa6]` and the unit
  is 4 B short. The same address also appears as `p[0xa6]` in the read - retail uses both
  materialisations in one function, so the pointer alone cannot serve both.
* **Head pointer colour.** With the stores addressed as `p + 0x32/0x30/0x88` mwcc puts
  `p` in r2 (retail r0) and the whole predicated block shifts colour. Writing them as
  `b + 0x4332/0x4330/0x4388` (same addresses) makes mwcc materialise `p` in r0 and the
  unit becomes byte-exact. `vprobe`: MATCH under b56; sp2 stays 132 B.

arm9 `0x02023df8` / `0x02023e58` (2x96) re-attempted from the matched sibling idiom
(`src/arm9/func_020237f8.c`): `u32 ip = 0x06600000;` then
`r = func_020268e0(data_02042370[0], a0, ip + a1, a2);` else `func_020269ec(a0, ip + a1, a2);`.
mwcc folds the constant to `add r2, r4, #0x6600000` (92 B - the immediate is encodable as
a rotated value), retail hoists `mov ip, #0x6600000` (96 B). u8*/s32 locals, the bare
literal and all 25 vendored builds (b56/sp2/base 92 B, sp3/sp4 84 B, 2.0/* 72 B,
dsi/* 68 B) never hoist it. Side finding: the dead `mov r3,r2` is `a2` surviving in r3 as
func_020268e0's 4th argument - the same 4-arg call the matched siblings make, so the shape
is right and the residue is only the constant hoist. Parked in `walls.txt`.

## 2.72 Mid-size leaf queue; two 108 B parks (2026-11-21)

Added `tools/match/leafq.py`: unmatched functions in a size band with no `bl`/`blx`,
excluding anything with a source under `src/` or an address in `walls.txt`, so the output
is a live queue rather than a stored list. Of the 96-320 B band it reports 124 leaves, 59
of them free of `ldm`/`stm`/`mul`/`mla`. The 100-400 B band is where most remaining bytes
sit, and leaves need no callee signatures.

Two 108 B leaves were derived to an exact body and parked in `walls.txt`:

* `0x0202ad9c` (sound-channel MMIO writer, five address groups). The body is certain; the
  residue is the pool layout - retail keeps 5 words with each base reused for a `+4`
  store, while every frameless spelling gives 7-8 words and every grouped spelling makes
  mwcc emit a phantom 8-byte frame.
* `0x0201aaa0` (`data_020475c4` + three writes to `0x04000470`). 104 of 108 B with the
  callee pointer cached; the missing instruction is retail's re-read of `a0[0]` between
  the two groups, and every spelling that re-reads it triggers the same phantom frame
  (136 B) as `0x02017954`.

Note for future attempts: on this ROM, a phantom `stmdb sp!,{lr}` + `sub sp,#4` with no
call and no `[sp]` access appears when a source mixes a cached pointer with a re-deref of
the same expression.

## 2.73 Two arm9 leaves from the Ghidra decompiler (2026-11-21)

Used the documented headless setup (`notes/ghidra-headless.md`) rather than
reading disassembly by hand: `analyzeHeadless ... -process arm9.bin -noanalysis -postScript
dump_decomp.py <addrs>`. Cheap (~10 s for four functions) and it hands back the expression
tree with load/store order already resolved, which is what took the most guessing before.

* `func_020221b8` (108 B) matched first try under b56 from the dump: `a0[1] = a0[1] + a1 * 4;`
  `a0[0] = a0[0] + 1;` then `if ((a0[0] & 3) != 0) return;`, the `a0[4]` block loading the old
  `a0[1]` into a pointer *before* bumping it, then `v = a0[1]; a0[1] = a0[1] + 4; a0[0] = v;`.
  No pool, no frame - the whole unit is 27 instructions.
* `func_02028f68` (124 B), 8-argument setter. **The 4-byte trap:** assigning back into the
  incoming parameter (`if (a6 == 0) a6 = ...; p[0xf] = a6;`) makes mwcc spill the 7th/8th
  (stack-passed) arguments back to their caller slots (`streq r1,[sp,#8]`) and reload them,
  costing 16 B. Writing the default as a ternary (`p[0xf] = a6 != 0 ? a6 : (u32)func_020296dc;`)
  or through a local temp keeps them in registers and matches. Both forms are byte-exact;
  the ternary is the one installed.

`0x0202ae08` (116 B, the getter twin of parked `0x0202ad9c`) decompiled cleanly but hit the
same wall: per-address `[0]`/`[1]` indexing turns the `+4` half into extra pool words (140 B),
a reused pointer folds it but adds the phantom frame (132 B). Parked with both spellings.

## 2.74 Why some complete units score below 100% in objdiff (2026-11-21)

The 33-unit gap between `complete_*` and `matched_*` is **not** a compiler-version
artifact. Diffing the two objects of `func_ov003_0204cc9c` settles it: 240 B of code are
byte-identical and the only differing bytes are the last pool word, where the base object
holds the literal `0x020eb720` and the delinked retail object holds a zero plus an
`R_ARM_ABS32` relocation. The source wrote `*(u8 **)0x020eb720` instead of the symbol
`data_ov003_020eb720`, so nothing marks the word as linker-filled; the ROM still matches
because the linker resolves both to the same value, but objdiff compares objects and sees a
literal against a relocation. This is the "reference globals by symbol" rule in AGENTS.md,
and it is a source fix, not a configuration change.

Verification note for this class: `fastmatch.compare` is the wrong oracle, because it
compares the object against the **ROM bytes**, where the symbol word is resolved. Compare
against the delinked object instead - `fm.masked(compiled, last_relocs) ==
fm.masked(text_of(delinks/<unit>.o), relocs_of(...))` - which is what objdiff does. The
whole-ROM digest stays the final gate after installing.

Ten units converted (raw address -> symbol, extern declarations added, object now identical
to the delinked one, digest unchanged): arm9 `0201d1c4` `0202e418` `02030a80`, ov003
`0204cc9c`, ov004 `0204e874` `02051148` `0205a63c` `0205a850`, ov005 `02051698`, ov008
`0204c7c0`. `matched_code` 100,652 -> 101,688 B and `matched_functions` 1,466 -> 1,476;
the gap is 2,016 B over 23 units.

Remaining 23, by pattern (for the next pass):

* Address passed as a call argument: ov003 `02050460` (`func_02025c94(0x020e6fe4, ...)`),
  ov009 `0204dc10`, ov004 `0205acfc` / `0205ca30` / `0205e274`, ov008 `0204d970` /
  `0204dec0` / `0204df38` / `0204e3fc` (nested `(u8 *)*(u32 *)0x...`), ov002 `02088c4c`,
  arm9 `02030888`, ov003 `02053b30`, ov005 `0204f42c`.
* No raw address in the source at all, so the residue is something else and needs its own
  diff: ov005 `0205a8dc`, ov003 `0205e028` / `02081ecc` / `0204e5bc`, ov004 `02053518`,
  ov005 `0204cf6c`, ov008 `0204ce64` / `0204d008`, ov009 `0204e8c0`, arm9 `02019aa0`.

Do not touch the objdiff configuration to close this; converting the sources to symbols is
the only honest route and it also makes the units link-safe.

## 2.75 The rest of the below-100% units (2026-11-21)

Two things made the first sweep miss 23 of the 33: the sources write the address
without a leading zero (`0x204f440`, `0x20eb720`, `0x2019e78`), which a `0x02......`
search never sees, and the address is often nested one level deeper than the simple
dereference, as `(u8 *)*(u32 *)0x020690a0` or a bare argument such as
`func_02025c94(0x020e6fe4, 0x800)`.

A second pass with a wider literal pattern and three shapes (dereference of a pointer to
pointer, dereference of pointer to value, bare address) moved 21 more units to 100%:
arm9 `02019aa0`, `02030888`; ov002 `02088c4c`; ov003 `0204e5bc`, `02053b30`, `0205e028`,
`02081ecc`; ov004 `02053518`, `0205acfc`, `0205ca30`, `0205e274`; ov005 `0204cf6c`,
`0204f42c`, `0205a8dc`; ov008 `0204ce64`, `0204d008`, `0204d970` (parked), `0204dec0`,
`0204df38`, `0204e3fc`; ov009 `0204dc10`, `0204e8c0`.

Two traps in the tooling, both worth remembering:

* The declaration text has to keep a space (`extern u32 name;`, not `extern u32name;`).
  A whole batch of candidates silently compiled to nothing because of it; when
  `compile_text` returns an empty blob, suspect the generated text, not the shape.
* `Path.read_text()` without `newline=""` translates CRLF to LF, so a `.replace` that
  anchors on a line ending silently does nothing. Every source in this tree is CRLF.

State: 1,497 of 1,499 complete units at 100%, 103,504 B matched (5.62%), 39.33% of
functions. The last two (`ov003/02050460` 92 B, `ov008/0204d970` 108 B) carry the correct
relocations already and differ only in register assignment / constant CSE; both are in
`tools/match/walls.txt` with the spellings tried.

## 2.76 Housekeeping after an outside review (2026-11-22)

A review of the repository by another model raised six points. Three were checked against the
tree and adopted:

* **`tools/match/walls.txt` was stale.** 28 of its 99 entries had a source that now scores
  100%, mostly b56 wins from 2.42 and the symbol-reference passes in 2.74/2.75. Pruned; 67
  entries remain (39 arm9, 12 ov002, 8 ov003, 4 ov005, 2 ov004, 2 ov008). The header now says
  that entries are probed under both 1.2/sp2 and 1.2/b56 unless the reason says otherwise.
* **`tools/match/true_fresh.txt` was stale.** It still listed 24 units that have sources,
  including ones matched in this session, so any "remaining work" figure taken from it was
  wrong. Regenerated by dropping entries whose `src/<mod>/<name>.c` exists: 2,263 remain.
* **Probe the neighbour's compiler first.** When a unit sits inside a band of known b56
  units, probe b56 before sp2 instead of after. First test (`arm9 0x02018330`, next to the b56
  win 0x02018318) came out 108B under *both* compilers, so the band hypothesis is not a
  universal explanation. It stays a cheap first probe, not a theory.

Three were held back, with reasons:

* Region-based compiler defaults in `configure.py`. Forbidden by AGENTS.md without the
  maintainer's explicit approval; the per-file list stays the mechanism.
* Merging adjacent delinks units, or re-marking "not code" ranges. Plausible for the branch
  stubs at 0x02033bc0/0x02033c0c and the cmp/bxeq fragment at 0x0203b2cc, but a wrong merge
  changes the link order and the digest gate would then reject the whole build; it needs a
  deliberate pass with a backup, not a side effect of wall triage.
* Converting the SDK walls to exact `asm` functions. AGENTS.md forbids generalising inline
  assembly into C but says delinked assembly stays assembly until the real C is found; whether
  an exact hand-written `asm` body is acceptable is a maintainer call, not an agent's.

Also worth keeping: the review's observation that the 0x158-stride records should become typed
structs is the same conclusion 2.74 reached from the other side, and is the natural next step
for readability *if* a struct spelling reproduces the same bytes - which is testable per unit.

## 2.77 The last below-100% unit, and why it looked like a register tie (2026-11-22)

`ov008 0x0204d970` was the only unit left under 100% (108 B, 99.81%). Its body was
instruction-for-instruction identical to retail except that `g` and `n` swapped registers, and
I had filed it as an allocation tie after roughly 35 spellings and all 25 compilers. That
diagnosis was wrong.

The source still contained `u8 *g = *(u8 **)0x0204f440;` - a raw address. Every attempt to
introduce the symbol rewrote the whole expression to `u8 *g = data_ov008_0204f440;`, and *that*
rewrite is what changed the allocation: the compiler sees a load of a pointer variable rather
than an address constant plus a dereference, and colours the intervals differently. So the
"tie" was manufactured by my own edit, not by mwcc.

The fix keeps the tree and changes only the operand:

```c
extern u8 data_ov008_0204f440[];          /* array => address constant, not a pointer var */
u8 *g = *(u8 **)data_ov008_0204f440;      /* same casts and dereference as before */
```

Byte-identical to retail, relocation present, digest unchanged. The same shape applies to the
other `*(T *)0x...` / `*(T **)0x...` conversions from 2.74 and 2.75: those already kept the
dereference (that is why they matched first time), and this unit was the one where the file had
never been rewritten at all.

State: 1,499 of 1,499 complete units at 100%, so `matched_code` == `complete_code` == 103,704 B
(5.63%) and `matched_functions` == `complete_units` == 1,499 (39.39% of 3,806).

## 2.78 First matches after the 100% milestone (2026-11-22)

Two new arm9 units, both the same family: a wait-for-condition poller.

`func_0202dd60` (100 B) and `func_02030520` (100 B) are `if (func_02027e98(chan, arg, 0) == 0)
return; do { WaitByLoop(x); } while (func_02027e98(chan, arg, 0) != 0);` with channel `0xe` vs
`0xd` and `WaitByLoop(a1)` vs `WaitByLoop(1)`. Both matched `1.2/sp2` on the first try once the
shape was right; mwcc hoists the two constants into callee-saved registers (`r5` = channel,
`r4` = 0) exactly as retail does, and the early `return` becomes the `addeq/popeq/bxeq` triple.

Worth recording: retail calls `WaitByLoop` with `blx`, but the compiler emits `bl` for a plain
`extern void WaitByLoop(u32);` call no matter how the callee is declared - it cannot know the
target is Thumb. The `blx` in the ROM is the linker's interworking fixup for a `kind:function(thumb)`
symbol, not a compiler choice. So a `bl` vs `blx` mismatch at a call site is not a source problem
and not something to chase; the earlier note about `CpuSet` "coming out as blx on its own" (2.68)
was describing that same linker step, and `func_020307c8` compiles to `bl` today while still
matching, which confirms it.

Park: `0x02017880` (104 B) list insert, written up in `walls.txt` - retail keeps two loads of the
offset halfword and two of `p[0]` where every spelling CSEs one of them.

State: 1,501 of 1,501 complete units at 100%, 103,904 B.

## 2.79 Two arm9 parks and a Ghidra note (2026-11-22)

`famsurvey.py arm9` has only five identical-skeleton groups left, and three of them are asm
thunks (`0x02033bd0`/`0x02033c1c` are `ldr r1,[pc]; bx r1` entry stubs) or already-documented
walls. The two real parks are written up in `walls.txt`:

* `0x02023df8` / `0x02023e58` (96 B, open-handle dispatch on `data_02042370`): straight 2- and
  3-parameter forms both give 88 B; retail keeps both arguments in callee-saved `r4`/`r5`, so
  something in the source makes `a0`/`a1` live across the branch.
* `0x02028920` / `0x02028958` (56 B): semantically identical code compiles to 56 B, but retail
  spends one more register (`push {r4,lr}` with `&e->f28` in `lr`, `e->obj` in `r4`) than my
  best shape needs. Since retail holds one more live value than the source appears to require,
  the source probably reads the field it updates a second time before the store.

Tooling note: the ov003 Ghidra project is `ATOJOV3`, but the program name inside it is not
`ov003.bin` - `-process "ov003.bin"` aborts with "Requested project program file(s) not found"
and the whole headless run takes several minutes before failing. `udis.py` covers ov003 fine,
so use that unless the program name is read out of the project first.

## 2.80 Data pilot: the pipeline works (2026-11-22)

First data unit matched: `src/arm9/data_02042240.c`, a 36-byte `u32[9]` = {1,2,3,4,5,6,8,9,10}
at the head of arm9 `.data` (retail then switches to strings, so the array is a clean object).
The delinks entry `complete` + `.data start:0x02042240 end:0x02042264` makes dsd emit
`data_02042240.o(.data)` between the gap objects, and `ninja sha1` still prints the retail
digest. objdiff scores it 100% with `matched_data 36`.

Two pipeline facts worth remembering:

* `build.ninja` regenerates only when `tools/configure.py` or `dsd.exe` changes. A new source
  file needs an explicit `python tools/configure.py ATOJ` first, otherwise the link fails with
  "Specified file ... .o not found" even though the LCF already names the object.
* `progress.py` used `complete_units` as a function-count fallback, which counted this data
  unit as a function (1,502 vs objdiff's `matched_functions 1,501`). Now the fallback only
  applies when the unit actually owns functions (`total_functions` non-zero).

The remaining question for a bulk data pass is not the pipeline but the shapes: pointer tables
need symbol references so the rows carry relocations, and large resource blocks may be better
kept as binary includes. Reported in the README as an open decision.

## 2.81 Data pilot 2: relocations in a pointer table (2026-11-22)

`src/arm9/data_02042384.c` is the case the first pilot did not cover: an 88-byte table of 22
function pointers (12 of them the same `func_02024890`, the rest the small `func_02024770`..
`0x020247e0` handlers). Written as `Handler data_02042384[22] = { func_..., ... };` with
`extern void func_...(void);` for each target, so every row carries an ABS32 relocation to a
function symbol rather than a copied literal.

Result: links at 0x02042384 with all 22 rows resolved to retail's addresses and `ninja sha1`
still prints the retail digest; objdiff reports `matched_data 88`, 100%. So the data pipeline
handles both directions - plain words and symbol-referencing tables - which is what a scripted
bulk pass needs.

Caveat for scripting: a run of pointer-shaped words is not proof of a table (many of them point
into `.rodata` strings or into `.data` itself). Only define a unit where the surrounding code or
an obvious table boundary makes the extent real, and let the digest decide.

## 2.82 One match, two allocation ties (2026-11-22)

`func_020251c0` (104 B) matched on `1.2/sp2`: `tok = func_02026350()`, the object behind
`data_02048d34`, `if (p) *p |= 1 << o->f6c`, `o->f64 = 0`, then `func_02025024()` and
`func_02026364(tok)`. The whole body matched instruction for instruction on the first try; only
the three call targets differed, which are relocations.

Two near-misses went to `walls.txt` instead of being committed:

* `0x0201f3d0` - retail derives a 0/-1 value from bit 2 of the flags word with `lsl #30` /
  `asrs #31` and passes it to `func_0201f38c` at both call sites. Any spelling that yields that
  value puts the bit in `r1` and the argument in `r2`; mwcc insists on the opposite. The
  variants that flip the registers (constant 0/1 arguments, `ands` truth test, signed bitfield)
  diverge in the test itself, so this is a genuine tie rather than a wrong source.
* `0x02025550` - sorted list insert, 104 B and instruction-identical except retail keeps `prev`
  in `ip` and `cur` in `lr` where mwcc always swaps them. Declaration order, assignment order,
  `for` form and comma expression all produce the same 104 B with the same swap.

State: 1,502 of 3,806 functions (39.46%), 104,008 B code (5.65%), 124 B data, 1,504 complete
units, all at 100%.

## 2.83 Teardown path, and the array-symbol rule again (2026-11-22)

`func_0201f0d0` (104 B, `1.2/sp2`) matched first try once the declaration matched retail's shape:
`(u32 id, u32 unused, T *t)` - the handle is the THIRD parameter (retail starts `mov r4, r2` and
never reads `r1`), and the shared pointer argument is `extern u8 data_02047f34[]` passed bare, so
it is an address constant loaded straight from the pool.

```c
if (id != t->f124) return;
func_02025b88(data_02047f34);
func_0201f38c(t);
t->f124 = 0; t->f128 = 0; t->f11c = 0;
if (t->f118 > 0) { func_0201cdb8(t); t->f118 = 0; }
func_02025b14(data_02047f34);
```

Same trap as 2.77, and worth stating as a rule: when the pool word holds an ADDRESS, the symbol
must be an array (`extern u8 sym[]`) and be used bare. Declaring `extern u32 sym;` and passing
`sym` emits an extra `ldr r0,[r0]` and breaks the match, because that form means "load the
pointer stored at sym".

State: 1,503 of 3,806 functions (39.49%), 104,112 B code (5.65%), 124 B data, 1,505 complete units.

## 2.84 ov003 engine-B init: shape found, counter register lost (2026-11-22)

The 3-member `180 B` family (`0x02066a54`, `0x020b85b8`, `0x020db6ec`) is an engine-B screen
init: two library calls, a DISPCNT read-modify-write at `0x04001000`
(`(v & 0xffcfffef) | 0x0100010`), then a 0x200-iteration loop that zeroes a u16 slot and writes
`0x200` as a word every fourth index; the other path reads the u16 at `g+0x86c2` and returns 0.

The reconstruction reaches the exact instruction sequence and length (180 B):

```c
u16 i;
if (mode == 0) {
    func_02009c8(8);
    func_02008b0(0x100);
    *(volatile u32 *)0x04001000 = (*(volatile u32 *)0x04001000 & 0xffcfffef) | 0x0100010;
    for (i = 0; i < 0x200; i++) {
        *(u16 *)(g + 0x8de8 + i * 2) = 0;
        if (i % 4 == 0)
            *(u32 *)(g + 0x8de8 + i * 2) = 0x200;
    }
} else {
    func_0204e46c(*(u16 *)(g + 0x86c2));
}
return 0;
```

Two findings from getting there: the conditional is `i % 4`, not `i & 1` (the loop writes a word
to a byte offset that grows by 2 per index, so only every fourth index is word-aligned), and the
counter must stay `u16` for `% 4` to produce retail's `lsr #31 / rsb / ror #30` sequence - `s16`
shifts by 31 instead and lands at 192 B, `int` at 184 B.

What blocks the match is the counter's register: retail keeps it in `r5` (hence
`push {r4,r5,lr}` plus a 4-byte frame, with the constant 0 in `lr`), mwcc puts it in `lr` and
drops the push. Written up in `walls.txt` for all three members - if one flips, the family is a
single substitution.

## 2.85 ov005 engine-B init: everything matches but one scratch pair (2026-11-22)

The 4-member `220 B` family reuses the ov003 shape with different offsets and an extra tail. Two
details worth keeping:

* Writing the loop address as `u8 *p = g + i * 2;` and then `*(u16 *)(p + 0x4a58)` is what makes
  the counter land in `r5` like retail (`push {r4,r5,lr}` plus a frame). Folding the constant into
  the expression (`g + 0x4a58 + i * 2`) instead hoists the constant, costs two instructions and
  puts the counter in `lr` - the same trap as the ov003 member.
* The mode!=0 path has no body: retail branches straight to the shared `return 0`, so the source is
  `if (mode == 0) { ...init... } return 0;` without an `else`.

What remains is one scratch pair. Retail zeroes into `lr` and keeps the loop pointer in `ip`;
mwcc does the reverse, and the two values are interchangeable, so the instruction stream is
identical apart from those two registers.

I swept all 25 vendored mwccarm builds on this candidate rather than assuming: `1.2/{b56,base,
sp2,sp2p3}` all produce the same swapped 220 B, `1.2/sp3` and `1.2/sp4` give 216 B, `2.0/*` 200 B
and `dsi/*` 204 B. None matches, so the overlay was not built with a different compiler build -
the divergence is in the source shape, exactly as the project concluded for the relocation class
earlier.

## 2.86 Six near-misses in a row: allocation, not shape (2026-11-22)

Since the ov005 family, every candidate has landed on the same failure: the instruction sequence
and length come out right and only the scratch registers differ.

| unit | size | divergence |
|---|---|---|
| arm9 `0x0201f3d0` | 104 | bit in `r1` / arg in `r2` swapped |
| arm9 `0x02025550` | 104 | `prev`/`cur` between `ip` and `lr` |
| ov003 `0x02066a54` x3 | 180 | loop counter `r5` vs `lr` |
| ov005 `0x020614a4` x4 | 220 | zero and pointer between `lr` and `ip` |
| arm9 `0x020101f8` | 80 | pool load / shift order, `r1r2r3` vs `r0r3r2` |

Two checks make this a source-shape problem rather than a toolchain one: the 1,503 units already
matched prove our flags and compiler reproduce retail's codegen in general, and a sweep of all 25
vendored mwccarm builds on the ov005 candidate gave 220/216/200/204 B with none matching.

One detail worth trying next time on `0x020101f8`: retail's first instruction is the pool load,
so its source referenced the global before the `idx * 8` shift, while mwcc hoists the shift above
the load. Anything that makes the global access syntactically earlier in the loop body may fix the
order; hoisting the chain into a local instead drops the function to 68 B, so it must stay inline.

## 2.87 Data pass rejected, ov002 bounds test parked (2026-11-22)

I considered continuing the data work as the alternative to the allocation wall and decided
against slicing more regions: defining anonymous arrays for whatever runs of small ints or
pointers happen to be aligned adds `matched_data` bytes without decompiling anything, and the
two pilots already proved the pipeline. Data should enter where the reading code gives the shape.

So I went to ov002, which is less explored. `0x0207ccd4` (80 B) is a six-argument bounds test
(`r0` mirror flag, `r1` pair, `r2`/`r3` x0/y0 in registers, `x1`/`y1` on the stack) that negates
the y bounds when the flag is 0 and returns whether the pair lies inside. Two spellings reach
exactly 80 B (`ok` flag with one `||` chain; `&&` chain with `return 1` last), and what remains
is the mirroring block's registers plus where `mov r0,#1` is placed.

Pattern so far: 7 near-misses, 0 matches since the last data unit, each within a handful of
register choices. Untried as a class: whether some source construct makes mwcc hoist a global
address above a loop-invariant shift (`0x020101f8`) - I probed four expression groupings and the
order never changed, so that lever is not source-expressible by parenthesis or operand order.

## 2.88 Argument pass-through shifts the whole allocation (2026-11-22)

`func_02029cc4` matched. It is a one-shot guard:

```c
void func_02029cc4(s32 a)
{
    if (data_0204abf8 != 0)
        return;
    data_0204abf8 = 1;
    func_02029d94(a);
}
```

Nothing about the body is unusual, but the *declaration* of the callee is: retail's first
use is `ldr r1, [pc]` / `ldr r2, [r1]`, not `r0`/`r1`. Writing `extern void func_02029d94(void)`
gives r0/r1 and a comparable-looking 60 bytes; writing `extern void func_02029d94(s32)` and
handing the incoming argument straight through reserves r0 for the call and pushes every
temporary up one register, which is exactly retail.

The general lesson is worth more than the one function: when an otherwise trivial body comes
out one register lower than retail across the board, look for a call that inherits an incoming
argument. `r0` being live into the `bl` is visible in the retail text as a call with no
preceding `mov r0, ...`, and that is the whole tell.

`0x020181b0` (88 B) came out instruction-for-instruction identical with one structural note:
the three list fields must be a nested `List` sub-struct addressed through an explicit
`List *l = &c->list` local, otherwise mwcc keeps `c` and uses 0x28/0x2c immediates instead of
retail's single `add r4, r0, #0x24` base. The only residue left is `l`/old-head swapping r4/r5,
so it is parked rather than installed.

I also corrected `progress.py`: it still claimed no data unit had a matching definition, which
has been false since `data_02042240.c` landed. The sentence now reports the matched data bytes.

## 2.89 0x02022554 is a four-argument function, not a delinks boundary (2026-11-22)

The entry had been parked as a suspected boundary because the body writes `r3` and reads `r1`/`r2`
before defining them. That reading was wrong: it is an ordinary four-argument function, and the
"undefined" registers are its parameters. Two things had hidden it — the body writes a state word
from `a3` before touching the others, and the delinked size includes the 12-byte literal pool, so
four of the trailing words look like code (`streq r0, [r0], #-0x60`) when they are actually
`{0x04000060, 0xcf7f, 0x0400035c}`.

The shape now reproduces all 18 instructions in retail's order with retail's registers. Both
details were needed: `v` as u32 (u16 inserts the lsl/lsr pair), and the guard inverted with the
mask in the `else`, which is what makes mwcc if-convert it to ldreq/strheq/bxeq instead of
branching.

What is left is two imm12s, and they come from literal-pool *ordering*: retail lays the pool out
{0x04000060, 0xcf7f, 0x0400035c}, mwcc {0x04000060, 0x0400035c, 0xcf7f}. Explicit right-hand
side, `&=`, and a mask temporary in the else branch all keep mwcc's order. This is the closest
near-miss in the project so far - 18 of 18 instructions, 2 of 4 pool slots - but the bytes still
differ, so it is not installed.

General note for boundary suspicions: check whether the "undefined" registers form a clean
argument sequence (r0..r3, then [sp], [sp+4]) before concluding the delinks range is wrong. Here
they did, exactly.

## 2.90 Forwarded argument and a seven-argument call (2026-11-22)

`func_02029f54` matched (76 B). Reading it as anything else failed by exactly the amount that
made it look hopeless:

```c
s32 func_02029f54(u32 a0, u32 a1, u32 a2, u32 a3)
{
    func_0202d5ec(data_0204ac00, a2, a1, a3, func_02029fa0, a0, 1);
    return 6;
}
```

The call passes seven arguments. The fourth is `a3`, forwarded untouched - which is why retail
has no instruction setting `r3` before the `bl`, and why a three-parameter version put the
callback in `r3` and shifted the stack words. `a0` and the constant 1 land in the sixth and
seventh slots. Two earlier readings both compiled to 76 B and both were wrong: one dereferenced
`data_0204ac00` twice, the other tried to pass a 12-byte struct by value (which pulls in the
whole `mov fp, sp` copy machinery, 112 B).

Two checks paid for themselves again: count the stack slots to infer arity (three words = three
arguments past `r3`, so seven total), and prefer "this argument is forwarded unchanged" over
"this `mov` is dead".

The `0x02023df8`/`0x02023e58` pair is the same lesson - the dead-looking `mov r3, r2` is a
forwarded fourth argument - but it stalls on a hoist: retail keeps the base in `ip` and adds it
per branch, mwcc folds the constant into each `add`, 92 B against 96. All 25 builds fold, so the
source is still not right rather than the compiler being different.

## 2.91 The 0x0202d158 pair: two deltas around a spin lock (2026-11-22)

`famsurvey arm9` lists five identical-skeleton groups; the largest is
`0x0202d158`/`0x0202d22c` at 212 bytes each. The body is fully decoded - six arguments, a
token/lock handshake, five context fields written, an optional callback arm, then a launch call
and a `*(u32 *)g->f0 == 0` result. Written out it compiles to 208 bytes with everything from
`func_02026364` onward identical, and exactly two deltas, both in the four instructions around
the spin lock:

1. retail tests the lock with `ldr r1, [r6, #0x34]` directly; mwcc emits `mov r1, r6` first,
   because it is CSE-ing the `f34` address against the `|= 4` that follows the loop;
2. retail performs that `|= 4` through a computed base (`add r2, r6, #0x34`), mwcc through the
   immediate offset form (`ldr r1,[r6,#0x34]` / `str r1,[r6,#0x34]`).

Both deltas are the same disagreement seen from two sides: whether the field address is worth a
register. Seven spellings were tried and recorded in walls.txt - pointer local at top (228),
pointer in a nested block (208), byte-offset cast (208), `volatile` cast (208), nested `Flags`
sub-struct (208), `for (;;)` with break (192), `goto` loop (192). Since every spelling that keeps
the shape also keeps mwcc's CSE, the source difference is probably not in these four lines at
all; the loop may not be a `while` over the flag but a call that returns the flag.

Worth noting for later: this is the biggest single family left (424 bytes), and the other three
groups from `famsurvey` are `0x0202d158`'s neighbours in size only - `0x020208b0`/`0x0202093c`
(140 each), `0x02033bd0`/`0x02033c1c` (60 each, pure inline-assembly CPSR wrappers, which the
hard rules keep as assembly), and `0x02028920`/`0x02028958` (56 each, already parked).

## 2.92 Two more families decoded, both blocked on register choice (2026-11-22)

`0x020208b0`/`0x0202093c` (140 B each) is now fully reconstructed and the whole control flow
matches: u16 pair update on `data_02048c74`, then a mode branch that clears bit 31 of
`0x04001000` at mode 0 or sets it and writes `0x83` to `0x04000249` at mode 0x100, then
`func_0201538(g->f0)`. Two details were needed: the guard must be written inverted
(`if (a0 != 0) { ... } else ...`) or mwcc if-converts the clear path instead of branching, and
the final call has to read through the global rather than the local pointer, which is what makes
mwcc emit retail's second pool load of the same address.

What is left is five instructions' worth of register naming at the top: retail computes `~a0`
into `ip` and keeps the two halfwords in `r2`/`r3`, mwcc uses `r3` for the complement and `ip`
for one halfword. Both vendored builds emit the identical five words, so it is not a version
choice, and four mask/declaration spellings do not move it.

This is the third family in a row (with `0x02017880` and `0x0202d158`) where the residue is a
scratch-register choice rather than a code shape. The common thread is `ip`: retail is willing to
park a value in `ip` across the arm9 register allocator's preferred low registers, and mwcc 1.2
avoids it whenever a low register is free. Nothing in the source shapes tried so far changes
that preference, so these are recorded rather than chased.

## 2.93 A new family decoded; both new parks are register/degeneracy walls (2026-11-22)

`famsurvey ov003 ov005` lists ten ov003 groups and three ov005 groups that have no source
(ov011 has no delinks file, so it is not surveyable). The smallest two were taken first.

`0x020d72f8`/`0x020d9f2c` (164 B each, 328 B total) is a module reset: two SDK calls, a
DISPCNT-sub mask, then a 0x200-entry halfword clear that writes a word 0x200 every fourth
iteration. Two useful levers came out of it:

- The early-out must be `if (a0 == 0) { body }` with one `return 0` after it. `if (a0 != 0)
  return 0;` makes mwcc duplicate the epilogue at the early exit (176 B instead of 164).
- A `u16` loop counter is what produces retail's signed-modulo idiom for `i % 4`
  (`lsr #31 / rsb r1, r2, r5, lsl #30 / adds r0, r2, r1, ror #30`): the counter promotes to
  `int` in `%`, while `i < 0x200` stays an unsigned `blo` because the counter itself is u16.
  This is a cheap way to tell "u16 counter" from "s32 counter" in future loops that use `%`.

Residue: retail keeps the hoisted zero constant in `lr` and the row pointer in `ip`; mwcc does
the reverse. Eight spellings (pointer local, separate assignment, `p += i*2`, `while`, u32 base
with `/4` index, function-scope `u32 z`, `u16 z`, separate `j = i*2`) all give 164 B with the
same swap, so this joins `0x020208b0`, `0x02017880` and `0x0202d158` in the scratch-preference
class: retail accepts `ip`, mwcc 1.2 prefers a low register and here prefers `ip` for a
different value.

`0x02082220`/`0x020c806c` (96 B each) is degenerate. Retail keeps a three-armed if/else-if
chain whose arms all assign the same constants (30, 60) and then tests the assigned value
against zero - a dead test. Written literally, mwcc folds the chain to one conditional pair and
drops the test (72 B). The trailing `lsl #16/asr #16` says the declared return is s32 with an
explicit `(s16)` cast. Same class as `0x02050f2c`.

`0x0202d158` was re-probed with five more spellings of the two deltas (pointer assigned just
before the RMW, u32-indexed base with `[0xd]`, a named flag local, a pointer reused by both
flag tests, and a load/modify/store through an explicit word variable). All five keep mwcc's
`mov r1, r6` + `ldr r0, [r1, #0x34]` and its immediate-offset `|=`, so the "loop condition is a
call returning the flag" idea remains untested but the pointer-spelling space is now closed.

## 2.94 Two matches; the compiler-band split shows up as address materialisation (2026-11-22)

`func_ov003_020ce31c` (104 B) matched under the default build on the first try: a u16 walk over
records 0x78..0x7b of a 0x1c-byte table at `data_ov003_020eb720 + 0x9fe8`, calling
`func_ov003_02052100((u16 *)p[4], (u16)p[0], (u16)p[1])`. The u16 counter is what reproduces
`mul r0, r5, r6` with `r6 = 0x1c` hoisted and the `lsl #16/lsr #16` increment.

`func_ov005_0206d778` (104 B) is a level-guard: if `*(u16 *)(b + 0x43a6) < 0x10` it stores 2 at
+0x36 and calls `func_ov005_0206f390(0)`, else it zeroes +0x4332, writes 0x44 at +0x4330 and
zeroes +0x4388; then it increments the guard halfword and returns 1. Two shape notes:

- The guard must be written inverted (`if (v < 0x10) { small + call } else { three stores }`).
  Written the other way round mwcc branches `blo` and leaves the three stores unpredicated; the
  inverted form gives retail's `movhs/strhhs ... bhs`.
- The tail was 4 bytes and one pool word short under 1.2/sp2: sp2 folds the increment into
  `add r1, r4, #0x4300` + `[r1, #0xa6]` twice, whereas retail pools 0x43a6 and materialises
  `add r2, r4, r1` once. `1.2/b56` emits exactly that, so the unit is recorded as a b56 unit in
  `config/ATOJ/mwcc_versions.txt`. Eight source spellings (u32 offset local, u16 index,
  pointer after the branch, compound assignment, volatile, u32 address temp) all fold under
  sp2 - the difference is the compiler band, not the expression.

The same-test cluster at `0x020d72f8`/`0x02066a54` is still blocked by the ip/lr preference
(§2.93); the 3x180 members have the identical loop and swap, so those five units (868 B) are a
single wall.

## 2.95 Four ov001/ov003 units match; the DTCM two-pool wall is confirmed again (2026-11-22)

`func_ov001_0204efa0` (88 B) matched first try. The table `data_0203d908` is a s16 sine table
whose element index is an *angle* divided by 8, and the source spells the angle as
`(0x20 - a0) << 9`: `idx = (angle << 9) >> 4; v = t[idx * 2]; v = (v << 8) >> 12;` then writes
`(v + 10) & 0x1ff` to 0x04000014 and `(0x100 - v) & 0x1ff` to 0x04000010. The `<< 9` plus
`>> 4` plus `lsl #2` chain is what `idx * 2` on a s16 table looks like; writing it as
`t[(0x20 - a0) * 64]` would not produce `mul`.

`func_ov001_0204ebb0` (152 B) matched first try as well: the `a0 > 0x40` arm builds the angle as
`0x28 - (a0 - 0x40)` (two instructions: `sub` then `rsb`, which is why the source must keep the
parenthesised subtraction), multiplies it by 0x199, does `>> 4` and `t[idx * 2]`, then
`(s16)v * 0x120 >> 12` and masks `<< 16` with 0x1ff0000 into 0x04000018 / 0x04001018.

`func_ov001_0204d8f4` (148 B) does not match for the known reason: the DTCM flag at
`data_027c0000 + 0x3ff8` is pooled by retail as two words and added, while mwcc 1.2 folds the
pair into the absolute 0x027c3ff8 and pools that. Four base spellings behave the same, so the
unit joins ov003 0x0205299c in that class.

## 2.96 A struct-typed record loop, and the naming layer proven on a mirror tree (2026-11-22)

`func_ov008_0204e60c` (76 B) is a fill loop over 0x80 records of 8 bytes, followed by
`func_ov008_0204cdac()` and `func_ov008_0204ca78(p)`. The flat spelling

```c
*(u32 *)(p + i * 8) = 0x200;
*(u16 *)(p + i * 8 + 6) = 0;
```

is the same length and differs from retail in two hoisted constants plus the address temporary
(mwcc `r1` = 0 / `r2` = 0x200, retail `r2` = 0 / `r3` = 0x200, with the second address in `r3`
against retail's `r1`). Declaring the record

```c
typedef struct { u32 a; u16 pad; u16 b; } Rec;
...
p[i].a = 0x200;
p[i].b = 0;
```

matches exactly. Reversing the two stores does not help: mwcc then reorders the stores
themselves (`strh` first) and the constants land in `r1`/`r2`. So the record layout is part of
the match, not a cosmetic annotation, and it is recorded in `docs/NAMING.md#struct-notes`
(u32 at +0, u16 pad at +4, u16 at +6).

`ov008 0x0204cc94` (140 B), the slot allocator that fills those records, does not match: retail
keeps `base + 0x40` in a register and loads the index byte twice before the guard, mwcc folds
the offset into `ldrb` and CSEs the two reads (136 B, or 144 B with a fifth register when the
table pointer is computed after the guard). Recorded in `tools/match/walls.txt`.

Naming: `tools/match/names.py`, `notes/naming/annotations.tsv` and `docs/NAMING.md` land as the
first step, with symbols unchanged. The rename was rehearsed on a mirror tree (copies of
`src`, `config`, `include`, `tools`, `extract` and `build/ATOJ`) - four matched units renamed,
`ninja sha1` unchanged, `t10.py --check` clean, `--verify` 0 stale, second run 0 rewrites - and
the mirror has been deleted. `--apply` refuses a tree containing `.git` without
`--allow-live-tree`, which is the gate that keeps the recorded names from being applied while
matching is under way.

## 2.97 Byte-field RMW: retail predicates the address, mwcc hoists or folds it (2026-11-22)

Three ov008 units in this session turned on the same four-byte question, so the class is
worth naming. For a read-modify-write on a byte field of a struct reached through a base
pointer, retail materialises the field address in a register (`add r2, r4, #0xe`) and, when the
RMW sits in a small `if`, the `add` carries the same condition as the `ldrb`/`strb`
(`addlo ... / ldrblo ... / addlo ... / strblo ...`). mwcc 1.2/sp2 does one of two other things:

- folds the offset straight into `ldrb [r4, #0xe]` / `strb [r4, #0xe]`, saving the `add`
  (112B and 128B spellings below), or
- materialises the address through an address-taken pointer (`u8 *p = &base[k];`) but hoists
  the `add` *above* the compare, so it is unconditional and only the load/store are predicated
  (136B, but the whole function minus that one instruction).

`volatile` on the field changes neither behaviour (it merely forces the reload). Declaring the
pointer inside the `if` block is folded away again.

Measured cases:

- `ov008 0x0204cf80` (136 B) - nearest spelling is exact except `add` vs `addlo` on the counter
  increment; see `tools/match/walls.txt`.
- `ov008 0x0204d9dc` (124 B) - both RMWs (`dir *= -1` inside the guard, `flags += dir` after it)
  want the predicated/inside-block address; mwcc hoists both `add`s above the compares (124 B
  but with a different allocation and a different division placement).
- `ov008 0x0204cc94` (140 B) - same residue in a non-branching form: retail loads the index
  byte twice and keeps `base + 0x40` in a register, mwcc CSEs the reads and folds the offset.

The lesson for the queue: a byte RMW through a struct pointer is not a shape problem. If the
only difference is an `add` that retail predicates and mwcc hoists, stop - no spelling reaches
it, and the wall note should say which side of the predicate the `add` ended up on.

## 2.98 ov004 instance table: scaled index needs the inline expression (2026-11-22)

`func_ov004_02050154` (104 B) matched once the table element was written as the *inline*
expression `((u32 *)(p + 0x20))[i]` instead of

- `*(u32 *)(p + 0x20 + i * 4)` - keeps a separate byte-offset register (`mov r3, ip` /
  `add r3, r3, #4`), 108 B;
- `u32 *tab = (u32 *)(p + 0x20); tab[i]` - hoists the base into `ip` outside the loop, 100 B
  with the wrong allocation;
- `u32 *tab = (u32 *)p; tab[i + 8]` - folds the +8 into the index (`add r2, lr, #8` plus a
  shifted-register load) instead of using the load's 0x20 displacement, 104 B.

The inline form is the only one that emits retail's `add r2, r0, lr, lsl #2` followed by
`ldr r2, [r2, #0x20]`. That is a general lesson for `base + const + i * 4` addressing: write
the pointer cast around the base-with-constant and index it, and do not name the intermediate.

The sibling `func_ov004_02050068` (100 B) gets the identical instruction sequence from the same
spelling and differs only in the register assignment (retail: count r2, cached id r3, counter
ip). Four spellings - inline count, cached count as a local, both as locals, retail's
declaration order - all pick the other assignment, so it is parked as a register tie.

`func_ov004_0204d8ec` (128 B) is the binary search over that same table; the shape is decoded
and written up in `tools/match/walls.txt`, and the only residue is mwcc duplicating the
`mov r0, #0x6b` epilogue where retail branches both exits to one copy.

## 2.99 Operand shifts are not CSE'd in retail, and early returns get if-converted (2026-11-22)

Two more ov004 spellings worth recording.

`func_ov004_0204fb74` (124 B, parked) repeats `n << 12` at four sites in retail, as an operand
shift (`add r0, r1, r4, lsl #12`, `sub ip, r1, r4, lsl #12`), so the half-extent never gets its
own register. mwcc CSEs the identical expression into `lsl ip, r3, #0xc` and then uses plain
registers, which costs 8 bytes (132 B). No spelling reproduced the repetition: a named `h` local,
an inline `n << 12`, and both with an `s32` or `u32` field pointer all gave 132 B. Operand-level
shifts are the one place where "the same expression four times" is cheaper than a variable.

The same unit shows the if-conversion boundary: `if (...) return 0;` three times in a row gets
if-converted into `movle r0,#0 / pople {r4,lr} / bxle lr` blocks (164 B) instead of branching to
a shared epilogue. Writing the conditions as one chained `&&` around a single result variable
drops it to 132 B and single-exit form, which is the shape retail has (`ble` to the shared
`mov r0,#0`), just with the CSE'd shift left over. So for a multi-clause range test, write one
chained condition and one result, never a chain of early returns.

The byte-width masking is worth copying too: retail emits four mask ops (`and 0x3f`, `and 0xff`,
`lsl 2`, `and 0xff`) because the source narrows twice - `u8 a = raw & 0x3f; u8 n = a << 2;`.
A single `u8 n = (raw & 0x3f) << 2;` emits only three.

## 3.00 The same RMW address class, now with mwcc short by one instruction (2026-11-22)

`ov004 0x02052808` (148 B, parked) finishes the picture from 2.97. Its loop body writes a
4-bit field in a 6-byte record with two statements:

```c
*p &= 0xfff;
*p |= (*(s16 *)(a1 + 0x3c) & 0xf) << 12;
```

Retail emits two halfword RMWs (`ldrh / and / strh`, then `ldrh / lsl #28 / orr ..., lsr #16 /
strh`) and materialises the field address once per iteration (`add lr, r3, #4`) so all three
accesses read `[lr]`. mwcc folds the +4 into the addressing (`ldrh [ip, #4]` ...) and lands at
144 B, four bytes *short* of retail. So the class cuts both ways: depending on the surrounding
pressure, mwcc either hoists the `add` out of the predicate (2.97, too long) or folds it away
(here, too short). In both cases the source cannot ask for the address register directly.

Two other results from the same unit:

- The loop bound must be re-read every iteration: retail ends with
  `ldrh r3, [r1, #0x90] / cmp r0, r3 / blt`. Hoisting it into a `u16 n` local adds a callee-saved
  register plus a stack slot (156 B). Write the field expression in the `for` condition.
- Writing the same field as a bitfield (`{u16 lo : 12; u16 hi : 4;}`) fuses the clear and the
  set into one RMW and drops to 124 B, so the two-statement spelling is the right one even
  though the field *is* a 4-bit field: retail's compiler did not fuse them, and matching means
  reproducing that.

## 3.01 Re-derived appends, and a reminder about re-matching matched units (2026-11-22)

`arm9 0x02018ee8` (148 B, parked) is the growable-array append that the ov004 overlay's `0x0204fc40`
and `0x0204fcf0` both need. Shape and evidence are in `tools/match/walls.txt`; the last 4 bytes
are the familiar pair - retail branches to one `mov r0, #0` epilogue and keeps the count address
in a register, mwcc if-converts the early return and folds the `+8`.

Worth recording as a process note: I started this batch on `arm9 0x02004e08` because `ready.py`
listed it as the callee blocking an ov004 unit, wrote a cleaner `rand()` body for it, and the
unit "matched" - but the file already existed and was already matched. The rewrite was
byte-equivalent churn on matched code and has been reverted. `ready.py`'s callee list can name a
symbol that a `src/` file already covers; check `ls src/<mod>/<name>.c` before writing a candidate
for a callee-sized address, not just the ready list.

## 3.02 Operand-shift fusion, and two units that need varargs (2026-11-22)

`arm9 0x02024b5c` (48 B, parked) is the interrupt-enable bit setter for the two enable words at
`0x027fffb0`. Retail builds the mask in a register (`mov r1,#0x80000000 / lsr r1,r1,r0`) and ORs
it plainly; mwcc always folds the variable shift into the `orr` operand
(`orr r0, r2, r1, lsr r0`), which is one instruction shorter and unfixable from the source -
naming the mask in a local does not stop it. This is 2.99's lever again (an expression that is
cheaper as a repeated operand shift in retail and as a CSE'd register in mwcc), so the class now
has both directions on record.

Two arm9 units near it are varargs functions and are out of reach of the current source style:

- `0x02034594` (116 B) builds a 12-byte `{buf, size, 0}` descriptor on the stack and calls
  `0x0203465c(&fmt, &descriptor)`, then terminates the buffer - a `vsnprintf`-shaped helper.
- `0x02034560` (52 B) is its wrapper: it passes `-1` as the size and the caller's vararg pointer.

Nothing in `src/` uses `...` or `va_list` today and `include/types.h` has no stdarg shim, so
matching these needs a decision about how to declare varargs first (mwcc accepts `<stdarg.h>`
from its own include tree, but the project has never taken that dependency). Worth settling
before spending iterations on either.

## 3.03 How many times the global is re-read sets the whole shape (2026-11-22)

`ov010 0x020514c8` (172 B, parked) is a useful calibration for a heuristic I have been using
implicitly. The three spellings differ only in how often `*(u8 **)data_ov010_0205e500` appears in
the source, and that alone moves the unit 136 -> 168 -> 196 bytes:

| spelling | global reads | size |
|---|---|---|
| one `g` local for the whole function, entry pointer too | 1 | 136 |
| `g` reloaded once per push, address written inline | 5 | 168 |
| every dereference written out (14 places) | 14 | 196 |

Retail sits at 172: it loads the global once for the count/index pair, again for each address
base, and once more for the second count - five reads, like the middle spelling, which is four
bytes short because the allocation lands one register differently (`push {r4,r5,lr}` + `sub sp,#4`
against `push {r4,lr}`). So when a unit is far off in *size* rather than in instructions, count
the global/table loads in retail first: the number of times a pointer expression is written in
the source is the cheapest size knob there is, and it is worth reproducing retail's count exactly
before chasing the last register.

The decode itself is in `tools/match/walls.txt` (ring buffer, 8-byte `{cmd, arg}` entries,
0x400-byte blocks, commands 0xc050008a / 0xc090808a with arg 0x5140).

## 3.04 Branch polarity again, and three ov002 near-misses (2026-11-22)

`ov002 0x020f2970` (84 B, parked) confirms the branch-inversion rule from the playbook with a
clean measurement: writing `if (v != 2) {fallthrough body} else {predicated body}` puts the
predication on the `ne` side, and swapping the two bodies (so the `== 2` body is the `if`) moves
it to the `eq` side - same size, opposite predicate. Retail wants the `ne` side predicated, i.e.
the `== 2` body written first. After that the unit is 84/84 bytes with identical instruction
order and only two register names swapped (`r0` vs `r1` for the address temp and the zero).

`ov002 0x020e4450` (84 B, parked) is the same family as 3.03 but on the *address* axis: retail
keeps the base in `r3` and the element address in `ip`, while mwcc folds both RMWs onto one
register. Adding the explicit `if (a1 == 0) return;` before them (matching retail's early
`cmp r1, #0`) recovers 8 of the 12 missing bytes.

`ov002 0x0204d488` (80 B, parked) is a size lesson in the other direction: retail addresses the
same u16 three times, splitting `0x30048` as `#0x30000 + [#0x48]` for two of them and pooling the
constant for the third. Naming the field pointer collapses all three into one register (56 B), so
the *repetition* of the full expression is load-bearing.

Also fixed: `tools/match/ready.py` now tolerates a trailing colon (and any junk) after the address
in a `walls.txt` line instead of crashing, which unblocked the ov002 and ov004 rosters - the two
malformed lines were section-internal notes rather than wall entries.

## 3.05 Copy the neighbouring module's spelling of an IO helper (2026-11-22)

`ov002 0x0204f654` (88 B) matched on the first attempt because it is the same hardware division
helper arm9 already has three of (`func_0201fd18`, `func_0201fd8c`, `func_0201fdc8`):

```c
    volatile u32 *p = (volatile u32 *)0x04000290;   /* DIV_NUMER */
    volatile u32 *q = (volatile u32 *)0x04000298;   /* DIV_DENOM */

    *(volatile u16 *)0x04000280 = 0;                /* arm9 writes 1 here, this one 0 */
    p[0] = a0;
    q[0] = a1;
    q[1] = 0;
    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    return *(volatile u32 *)0x040002a0;             /* DIV_RESULT low word */
```

Two things made it a one-shot. First, the pool words decoded to `0x04000280/90/98/a0`, which is
the divider block, and the project already has a matched spelling for that block - including the
`<volatile u16>` cast on the status register that is required for the busy loop not to be hoisted.
Second, the arm9 versions differ from this one only in the control write (1 vs 0) and in how much
of the 64-bit result they read, so the shape was already proven. Before decoding an unknown IO
helper from scratch, grep the matched tree for its register address: the sibling function usually
hands over the exact local-pointer style mwcc needs.

## 3.06 The aligned-read idiom, and where if-conversion refuses to be argued with (2026-11-22)

Five arm9 units decoded this session, none installable, but two reusable results.

**The unaligned-u8 read.** `0x02032c38` (56 B, `strlen` over a pointer that may be odd) and
`0x02032b00` (48 B) both read a byte through a u16 load chosen on pointer parity. This source
reproduces retail's sequence exactly:

```c
    if ((u32)p & 1)
        v = (u32)((*(s16 *)(p - 1) & 0xff00) >> 8);
    else
        v = *(u16 *)p & 0xff;
```

`ands r1, r0, #1` picks the predicate; both arms fold into `ldrheq` + `and #0xff` and
`ldrhne [r0, #-1]` + `and #0xff00` + `asr #8`. The `s16` cast matters: with `u16` mwcc emits
`lsr`, retail has `asr`.

**If-conversion of a two-instruction loop body.** The same unit then refuses to close. Retail's
loop is

```
008 add r2, r2, #1      ; n++
00c add r0, r0, #1      ; p++
010 ...read...
02c bne 008
```

while mwcc always predicates the two increments (`addne r2`/`addne r0`/`bne 008`) and saves the
four bytes, for 52 B against 56 B. Writing the exit as `break`, as an early `return n`, or as a
`goto done` changes only the *epilogue* (`moveq r0, r2`/`bxeq lr` versus a shared tail); the body
stays predicated in all three. The exit shape is therefore a real lever (early return lengthens
the unit by exactly 4 B and moves the epilogue), but the body predication is not, at this body
size.

**Divider family.** `0x0201fd50` (60 B) is the reciprocal starter - `DIVCNT = 1`,
`NUMER = 0:0x1000`, `DENOM = a0:0` - and the matched `func_0201fd18`/`func_0201fd8c` gave the
spelling directly. It is 60/60 with the constant and the NUMER pointer swapping r2/r3, the
allocation tie the playbook warns about.

**Add-and-clamp pair.** `0x0200fbc0`/`0x0200fc18` (64 B each) add to a field then clamp it from a
pooled bound (0xc350, and 0x80bbb040 - a float bit pattern used as an integer bound) reached
through `*(u32 *)(*(u8 **)(data_02042a68 + 0x10) + 0x154)`. The two halves want opposite address
materialisation: retail materialises `+2` into `ip` for the RMW and uses the displacement for the
clamp, and any single spelling that gets one half right CSEs the other into the same form.

## 3.07 Signedness picks HS versus GE, a new cheap lever (2026-11-22)

`ov002 0x0208c98c` (104 B, **matched**) is a bounds-checked setter: reject `a0 >= 2` and
`a1 >= 4`, then write two u16 at `base + 0xba + a0 * 0x30 + a1 * 0xc + {4,6}`.

The unit was 104/104 on the first structural attempt but with `addge`/`popge` where retail has
`addhs`/`ldmhs`. That is not a disassembler synonym: `hs` is the unsigned condition (cond 2) and
`ge` is the signed one (cond 0xA), so the encodings differ by one nibble per instruction.
**Declaring the range-checked parameters `u32` instead of `s32` turned the whole unit into a
match** - the comparison then compiles as unsigned `>=`, which is `hs`. Any guard written as
`if (x >= CONST) return;` on an index or count should be checked for this first: `s32` and `u32`
give the same size, so `var3` shows it only as a condition-name difference on otherwise
identical instructions, and it is the cheapest possible fix.

The other half of the same unit is a size lesson: the base pointer has to be initialised at its
declaration, *before* the two guards (`u8 *base = *(u8 **)data_ov002_02102070 + 0x4bc4;`).
Declaring it below the guards makes mwcc take a callee-saved register and the unit shrinks to
84B, because it then keeps the whole computed address in r4 across the checks instead of
rematerialising from lr/ip after them.

## 3.08 Three ov002 near-misses around the same entity table (2026-11-22)

`0x02066080`, `0x020799d4` and `0x0207b494` (112 B each, all parked) are all 112/112 or one
addressing form away. What they teach:

- **A reload needs `volatile`.** In `0x02066080` the source legitimately reads the same field
  twice (compare, then increment). Plain C lets mwcc CSE that into one register and the unit comes
  out 4B short (108B). Writing the field as `<volatile u32>` forces retail's two loads and lands
  112B. If a near-miss is short by exactly the size of one load *of a value already in a
  register*, try `volatile` on that access.
- **Compute the entity pointer before a local struct copy.** `T12 t = *(T12 *)data_...;` placed
  before `b = ... + a0 * 0x38` makes mwcc allocate the copy from r2; placing it after gives
  retail's `mul lr, r0, r1` / `ldm r0, {r0, r1, r2}` order exactly.
- **Two statements beat one expression when the offset is scaled.** `e = base + 0x274; e = e +
  idx * 0x1c;` reproduces retail's `mla lr, r4, r3, lr`; the single nested expression picks a
  different register for the scaling and shuffles everything after it.
- The shared dead end is the pre-indexed writeback form `ldr rX, [rY, #imm]!`. Retail uses it for
  the RMW half of two of these units and mwcc never emits it from C here (offset form twice
  instead, same size, different encoding). That is the one residue left in all three.

## 3.09 Two more ov002 matches: block scope and the dead-store mask (2026-11-22)

`0x02089484` (108 B) and `0x0207fa7c` (108 B) both matched after the same style of correction.

**`0x02089484`** is a pure field initialiser (no branches): set 0x300 in a word, copy two u16 from
`base + 0x4bc4 + 0x300 + {0x58,0x5a}`, zero four others, and store `a0 & 0x1ff`. The single
correction was `b += 0x300;` as its own statement. Writing the offset into the pointer
construction (`*(u8 **)data_... + 0x4bc4 + 0x300`) makes mwcc fold it into the *pool literal*
(`0x4ec4`) and the unit is one `add` short; a separate `p = b + 0x300;` local instead puts the
result in `ip` where retail uses the base register itself, and only the compound update on the
existing pointer reproduces `add r1, r1, #0x300`.

**`0x0207fa7c`** needed three things, in this order:
1. the early exit is on the **clear** bit (`if (!(a1 & 0x800000)) return;`); writing the set test
   gives `ne` where retail predicates `eq`;
2. the boolean argument is masked to a byte, which only happens if the value is a `u8`;
3. **the `u8` local must be declared inside the `if` block, not at function scope.** At function
   scope mwcc hoists the `cmp`/`moveq`/`movne`/`and` sequence above the branch and the unit is
   4 B short; inside the block it lands exactly where retail has it. If a masked boolean argument
   appears before a call in retail but your build computes it earlier, move the declaration into
   the innermost block that uses it.

Also confirmed again on `0x0207b41c`: `(u32)*(u8 *)p >> 1` gives `lsr`, plain `*(u8 *)p >> 1`
promotes to signed int and gives `asr` - the same signed/unsigned trap as 3.07, this time on the
shift rather than the comparison condition.

## 3.10 A switch on a mode byte, and three hits on the predication wall (2026-11-22)

`0x02083f0c` (128 B, **matched**) is a small mode-dispatch: increment a per-index byte at
`base + 0x36b3 + a0`, then switch on `*(u8 *)(base + 0x8f)` - case 0 returns 1, cases 1 and 2
return whether the counter just reached 2 or 3, default returns 0. It compiled correctly first
time with three separate `return` statements inside the `switch`, and the counter written as
`*(u8 *)(b + a0) = *(u8 *)(b + a0) + 1` rather than `++` (retail loads into r1, adds, then
stores through the same `[r2, r0]` addressing).

The three parked units this round all fail on the same axis and it is worth naming: **a predicated
store block (`strbeq`/`strbne`) cannot be turned into retail's branch-and-two-addresses by any
wording tried so far.** `0x02084b28` wants `bne` to an else block that re-materialises
`add r0, r4, r3`; every spelling gives either `strbeq`+`strbne` at 120B or a hoisted address at
128B with the wrong register. `0x0208a034` and `0x020617d0` fail the same way on a decrement and on
a loop-exit pair. When a unit is short by exactly the branches' worth of bytes and the diff shows
`eq`/`ne` predicates where retail has a branch plus a duplicated address computation, stop - this
is the wall the playbook describes, and it has now been confirmed from both directions (predicated
here, branched in retail) in five different units.

## 3.11 A scaled-index copy, and a near-miss that is pure register naming (2026-11-22)

`0x020638f4` (88 B, **matched**) is the leanest member of the 12-byte-table family:

```c
    u8 *b = *(u8 **)data_ov002_02102070 + 0x1744 + a0 * 0x38;
    T12 t = *(T12 *)data_ov002_020fdc30;
    *(u32 *)(b + 0x18) = t.v[*b];
```

It confirms the ordering rule from 3.08 from the other side: here the entity pointer can be
built first because the table copy is a plain initialiser that mwcc schedules after the `mul`
anyway, and the `ldm`/`stm` pair comes out exactly as retail has it. The same shape at
`0x02066080` needed `<volatile u32>` on the field because it reads it twice; this one writes once
and needs nothing.

`0x02068c78` (64 B) is parked at full size with every register renamed: retail keeps the base in
`r1` and a copy in `ip` because it needs the base twice (`mla r2, r0, r2, ip` to index the entity,
`add r1, ip, r1` to reach the target block), while mwcc keeps one copy in `r3` and offsets the
whole allocation by one. Two useful details did come out of it, both of which changed the byte
count rather than the registers: the shift needs `(u32)*(u8 *)p >> 1` (otherwise `asr`), and the
offset chain has to be collapsed into a named pointer once (`p = base + 0x1e6c + i;` then
`p[0x680] = ...`), because writing `t[i + 0x680]` makes mwcc recompute the address for each of the
three stores and the unit grows to 100 B.

## 3.12 Three-way OR guard matches; frame elimination is not reachable (2026-11-22)

`0x0207be2c` (72 B, **matched**) writes three zero bytes when a per-index mode byte is one of
three values:

```c
    u8 v = *(u8 *)(base + 0x3d + a0);
    if (v == 0xc || v == 0xd || v == 0x11) {
        *(u8 *)(base + 0x3f + a0) = 0;
        *(u8 *)(base + 0x3d + a0) = 0;
        *(u8 *)(base + 0x43 + a0) = 0;
    }
```

It matched first time, and the shape is worth noting: retail tests 0xc and 0xd with two `beq`s to
the same block and then `cmp #0x11; bxne lr`, i.e. a chained `||` in a single `if` with a
fall-through return - not a `switch`, which would have produced a different test order.

`0x0204f98c` (104 B, parked) is the clearest frame-elimination wall so far. Retail has no prologue
at all and returns with bare `bxeq`/`bxne`; the body needs the base plus four `s16` loads plus two
differences live at once. Every spelling tried - two locals, four locals, one nested expression,
a single `&&` chain - makes mwcc emit `stmdb {lr}` + `sub sp, #4` and land at 128 B. When retail is
frameless and your build is not, the gap is not the arithmetic: it is the register budget, and no
formulation of the same computation changes it.

`0x02082090` (96 B, parked) reached full size with the `base + K` pointer declared at the top of
the function - which is now the third unit where an early-computed `base + const` pointer is the
difference between 88 and 96 bytes - but keeps a `bx lr` where retail has `bxne` after the
m==1 test.

## 3.13 Four more ov002 units, all short by a materialisation (2026-11-22)

Nothing landed this round; four units were decoded to the last instruction and each is a few bytes
from matching for the same two reasons already on record. Recorded compactly because the individual
diagnoses are more useful than the fact they failed:

| unit | size | gap | why |
|---|---|---|---|
| `0x020e198c` | 128 | 124 B | retail uses `add r3, r0, #0xb8` then `[r3]` for the OR and `[r0, #0xb8]` for the later read; every spelling folds both |
| `0x02065ffc` | 132 | 132 B, wrong frame | reaches full size with `T12 t; t = *(T12 *)table;` (a plain initialiser lets mwcc fold the copy, 116 B), but keeps `stmdb {lr}` + `sub sp, #0xc` where retail is a bare `sub sp, #0x10` |
| `0x02052608` | 72 | 64 B | tail call reproduces (`ldr ip, [pc]; bx ip` for `return f(*p);`), but mwcc folds the address into `add #0x1000` + `[r1, #0x8dc]` instead of retail's pooled-0x1824 add plus `add r2, r0, #0xb8` |
| `0x0204f5a0` | 132 | 144 B | retail reads the index byte three times and uses `ldr rX, [ip, rX, lsl #2]` operand shifts for both table accesses |

Two things worth carrying forward. First, `0x02065ffc` adds a second confirmed instance of the
rule that a **two-step struct assignment forces the stack copy** that a declaration-with-initialiser
lets the compiler fold - the same lesson as the pointer-initialisation cases, but for `ldm`/`stm`
copying rather than address arithmetic. Second, `0x02052608` shows that mwcc *does* emit retail's
`ldr ip, [pc]; bx ip` interworking tail call from an ordinary `return f(x);`, so that particular
retail idiom is not a sign of a function pointer and should not be read as one.

## 3.14 `volatile` on a reloaded byte, and the empty overlay (2026-11-22)

The duplicate-load lever generalises past words. `ov008 0x0204cd20` reads the same index byte
twice - once for the count-table lookup, once for the record stride - and retail emits two `ldrb`s.
`u32 n = *(u8 *)t;` followed by `u8 *e = c + 0x610 + *(u8 *)t * 0x300;` makes mwcc common the two
loads into one and come up 4 bytes short at 136; changing only the second read to
`*(volatile u8 *)t` restores the second `ldrb` and lands on retail's 140 bytes exactly. So the rule
from the word cases holds for sub-word reloads as well: when retail loads the same location twice
and the C form will not, `volatile` is the spelling, not a second named temporary.

The unit still does not match. With the size right, the only residue is that retail materialises
`add r3, r4, #0x40` and addresses the byte through `[r3]`, where mwcc folds both reads to
`[base, #0x40]` and hoists the count lookup ahead of the record pointer. That is the same split
materialisation seen at `0x020e198c`, now confirmed in a second overlay and a different shape, which
makes it the single most common remaining gap rather than an ov002 quirk.

Two bookkeeping findings while picking work. `ov007` has no code at all - its overlay is 32 bytes
and its delinks file contains only `.ctor` and `.bss` - so `auto.set_module` now reports that
plainly instead of raising `AttributeError` on the missing `.text` match. And the 12-byte
`ov006 0x0204df9c` is a boundary artefact, not a function: it pops a frame it never pushes, so the
delink should start one `push` earlier. Both are recorded in `walls.txt`; neither changes any
source.

A third shape joined the list while picking the next unit: `ov002 0x02082034` wants its index byte
re-read inside a four-iteration loop (retail's loop body begins at the `ldrb`) while still sharing a
single induction register between the `lsl #3` destination and the `lsl #1` source. Hoisting the read
costs nothing but produces 96 bytes with two extra induction registers; inlining it lands at 112.
So the `volatile` lever from above is not interchangeable with "move the expression into the loop":
mwcc strength-reduces differently in each case, and neither matches here.

## 3.15 The closest wall yet: five units, one swapped register pair (2026-11-22)

`ov003 0x02066a54` (180B, family of three) and `0x020d72f8` (164B, family of two) are one source:
display init that clears the resident pointer, calls `func_020209c8(8)` and `func_020208b0(0x100)`,
rewrites `*(u32 *)0x04001000` to `(old & 0xffcfffef) | 0x100010`, then loops 0x200 times zeroing the
u16 at `b + 0x8de8 + i * 2` and stamping `0x200` every fourth entry; the 180B members take an else
branch calling `func_0204e46c(*(u16 *)(b + 0x86c2))`.

Both size classes reproduce exactly - 164/164 and 180/180 - with every opcode in retail's slot. The
entire residue is one register pair: retail holds the zero store value in `lr` and the rolling
address in `ip`, mwcc emits `mov ip, r5` and `add lr, r4, r5, lsl #1`. Nothing else differs, not even
the `% 4` sequence (`lsr`/`rsb`/`adds ... ror #30` plus the predicated `addeq`/`streq`).

Five spellings do not move it: a literal 0, a `u16 z = 0` variable carried across the loop, `u8 *p`
declared inside the loop, the same declared outside and assigned inside, and a pre-offset
`u16 *q = (u16 *)(b + 0x8de8)` indexed as `q[i]` (which also folds the split 0x8d00/#0xe8
materialisation and drops to 176B). `vprobe` confirms 1.2/b56 and 1.2/sp2 make the same choice, so
the version-override route is closed here too. Both builds create the zero temporary first and then
the address, and differ only in which of r12/r14 they assign to the first - so this is the allocator's
scratch preference, not an evaluation-order effect that source can reach.

Worth remembering as the highest-value retry in the tree: five units and 868 bytes behind a single
register name. If a lever for scratch-register preference ever appears, this family cashes in
immediately.

## 3.16 Moving to arm9, and what `volatile` is actually good for here (2026-11-22)

The arm9 module has small identical-skeleton groups the ov002 sweep never touched, so three were
taken in one pass. None matched, but two produced exact-size bodies and one generalises the
`volatile` lever in a way worth writing down.

`arm9 0x02028920` is an indirect-call trampoline: it accumulates `*(u32 *)(s + 0x28) +=
*(u32 *)(s + 0x34)` and calls through `*(void **)(*(u8 **)(s + 8) + 0x40)` passing the object,
`*(u32 *)(s + 0x2c)` and the **pre-update** value of `s + 0x28`. Retail loads `s + 0x28` twice before
the store, which no plain C expression reproduces - mwcc commons the two loads. Marking the two
reads and the `+ 0x34` read `volatile` reproduces retail's load order exactly, `0x34, 0x28, 0x8,
0x28, 0x2c`, and the double load with it. So `volatile` is not just a reload lever for a single
re-read value: applied to a small group of adjacent reads it also pins their schedule, which is the
only reliable way found so far to make mwcc emit a load order retail chose.

What is left there is 4 bytes of address form: retail materialises `add lr, r0, #0x28` first and
addresses the load and the store through `[lr]`, keeping the trampoline object in r4 and pushing
`{r4, lr}`; mwcc folds every access to `[r0, #0x28]`, keeps the object in ip, and pays for a
`stmdb {lr}` frame because lr becomes a temporary. Same split-materialisation wall as
`0x020e198c` and `0x0204cd20`, now in arm9.

`arm9 0x02023df8` is a cleaner near-miss: a void `if/else` forwarding to `func_020268e0(handle, a0,
0x6600000 + a1, a2)` or `func_020269ec(a0, 0x6600000 + a1, a2)`. Branch layout, argument order and
both `bl`-then-epilogue tails are right; the 4-byte gap is that retail hoists `mov ip, #0x6600000`
once before the `beq` while mwcc folds the constant into each `add` as an immediate. That is the
reverse of the usual case - retail *worse* than mwcc - and no spelling (inline constant, named
local) stops the fold.

`arm9 0x02033bd0` is not writable from C at all: its first two instructions are `ldr r1, [pc]; bx r1`,
a jump into the real body, which the delink treated as the function start. Left as assembly.

## 3.17 A new wall class: mwcc copies parameter-derived values, retail updated in place (2026-11-22)

`arm9 0x020255e8` is 100 bytes of straight-line struct init and it matches at 100/100 with every
operand identical. Three instructions disagree, and they show a pattern the earlier walls did not:
retail **modifies the incoming parameter register in place** -

    add r1, r1, #4        add r1, r1, #1  (later)
    sub r2, r2, #0x40

- while mwcc copies each computed value into a fresh register and updates nothing -

    add r3, r1, #4
    sub r1, r2, #0x40
    ands r1, r3, #1

Three spellings give the identical copy: a named local for the sum, compound assignment on the
parameters (`a1 += 4`, `a2 -= 0x40`, which is the documented way to get `add rX, rX, #c`), and fully
inline expressions. Both compiler builds agree, so it is not a version lead. In the documented case
the compound form lands in place; here mwcc refuses even when the parameter is dead after the add, so
the rule has an exception tied to how the value is then consumed (the here: a ternary on the result).
Worth watching for: this is the first unit where the *only* difference is that retail reused an
argument register.

Two roster findings from the same sweep. `autorun.py` is now exhausted: with `tried.txt` at 1,238
entries it reports `candidates 0` for arm9 (55 bailed) and for ov003/ov005/ov006/ov008/ov009/ov010,
so nothing is left to farm automatically and everything remaining is manual. And `ov000` is a second
empty overlay like `ov007` - no `.text`, 0 functions - which the new guard in `auto.set_module`
reports cleanly.

## 3.18 Four units from the clean-shape queue (2026-11-22)

The 550-unit clean queue (96-320B, no calls, no multiplies) paid off: four matches in one pass, and
one of them gives a lever for a wall class that had looked hopeless.

`ov002 0x0204d4d8` (100B) clamps a value into a range chosen by `a0 < 0x81`. Two levers were needed:

1. The parameter must be **`s32`, not `u32`**, or the range test comes out `movlo/movhs` where retail
   has `movlt/movge`.
2. The accumulation must be written **left-to-right from the low end**: `v = a1 + lo + f(...)` makes
   mwcc precompute `a1 + lo` in a spare register, while `v = lo + f(...) + a1` chains
   `add r0, r5, r0` / `add r0, r6, r0` exactly as retail does.
3. The second clamp had to be a **plain assignment, not a return**: `if (v < lo) v = lo; return v;`
   emits `cmp` + `movlt r0, r5` + a shared epilogue, where `return lo;` made mwcc fold the whole tail
   into a `max()` (`movge r5, r0` + `mov r0, r5`) and grow the unit to 104B. The first clamp keeps
   its `return hi;`, which is what retail does.

`ov003 0x02067020` (100B) is a jump-table switch. Writing only the live cases (`8`, `9`) produced two
compares and 72B; retail has a full `addls pc, pc, r1, lsl #2` table with ten entries, seven of which
land on the epilogue. Listing the dead cases explicitly (`case 0:` through `case 7:` each falling out
of the switch) is what makes mwcc emit the table - so a table with identical targets is a sign of
**enumerated dead cases**, not of an optimisation.

`ov002 0x02084244` (100B) settled a register pair by **declaration order**. Retail keeps the raw
global base in r4 and the derived `base + 0x36ac` in r5. Declaring `base` first and `s = base + 0x36ac`
second gave the opposite pair and a 100/100 unit that still differed on every line; swapping the two
declarations - `s` first, `base` second, each reading the global - put them in retail's registers and
matched immediately. When two pointers derived from the same global swap registers, try the reverse
declaration order before anything else.

`ov002 0x02057e58` (100B) matched on the first attempt: a per-entity lookup with `mla` strides 0x158
and 0x1c, a `(a1 == 0x55) ? 0x56 : 0x55` toggle guarded by `idx > 1`, and two tail calls.

Also tried and parked: `ov004 0x02050068` (100B) is five instructions long at 108B because mwcc
strength-reduces the `self + 0x20 + i * 4` element access into a second induction variable
(`add r3, r3, #4`) where retail recomputes `add r0, r4, ip, lsl #2` every iteration; and
`ov002 0x0204f70c` needs its magic-division constant (`0xa2f96525`, paired with an
`if (v < 0) v += 0x6488` wrap where 0x6488 is 65536*pi/8) identified before it can be written.

## 3.19 The first-store split, and a clean ov008 sequence match (2026-11-22)

`ov008 0x0204e4b0` (108B) matched on the first attempt: a flat init sequence of eleven calls -
`func_ov008_0204ed28`, `0204e554`, `0204df5c`, `0204c9ac`, `0204e434`, `0204e75c(0, 0x80)` - then
`b[8] = 0`, `0204c8dc(1)`, `0204e3fc`, `0204c894(1)`, `0204c810`, `0204ebc8`, and
`*(u32 *)0x04000540 = 1`. Nothing needed adjusting; long call sequences with a couple of side stores
stay cheap. Note that every callee is `func_ov008_*`, not `func_0204*` - the overlay's own namespace,
which `var3` masks and only `reg.py` verifies.

Three units parked, and they share one signature that is now the most common residue in the queue:
**mwcc splits the address of the first access**. `0x02069b3c` and `0x02086f14` both want
`ptr = base + K` materialised once and every later access addressed through it; mwcc instead encodes
the first access as `add rX, base, #hi` plus a `[rX, #lo]` displacement and materialises `base + K`
only when a second use appears. That costs one instruction and breaks the match. Neither a named
pointer, a two-step declaration-then-assignment, a load/orr/store split, nor a padded struct type
stops it - a full list of failures for the two units is in `walls.txt`. The pattern to watch for:
retail's first access after materialising a base is a store (`strb`), and the split only ever happens
when that store is the first use.

`0x02064b04` adds a second axis to the same problem: the closest spelling (104B) lets mwcc schedule
the `mul` after the call and keep the index in r4, while retail computes it first and keeps the
argument in r5. Branch polarity in the state dispatch also has to be inverted - retail puts the
`st == 1` arm out of line behind a `beq` and leaves `st == 2` inline.

## 3.20 A symbol-type trap, and where a pointer declaration lands (2026-11-22)

`ov002 0x02076658` (108B) matched: a spawn helper that rejects `a0 >= *(u16 *)(data_ov002_02102074 +
0x38)`, then calls `func_ov002_02077974(func_ov002_02076358, 0x14, 0, 0, 0)` and, on success, writes
1 at +6, `a0` at +7 and `a1` at +5 of the returned block.

Two traps cost a round trip each, and both are worth remembering because `var3` cannot see either:

1. `ldrh r0, [r2, #0x38]` after `ldr r2, [pc]` means the pool word is **the object itself**, not a
   pointer holder. Every other ov002 unit in this batch dereferences `data_ov002_02102070`; here the
   symbol is `data_ov002_02102074` and is read directly. Writing `*(u8 **)` produced `ldr r0, [r2]`
   and diverged immediately. Always check whether the pooled address is a container or a pointer
   before deciding the dereference level.
2. The same pool word `0x02076358` is **a function address used as an argument**
   (`func_ov002_02076358`, size 0x300), not a data buffer. Declaring it as `extern u8 data_[]` makes
   `var3` say MATCH and `reg.py` fail at link, because a data extern adds a relocation the linker
   cannot resolve to a code symbol. Grep `symbols.txt` for the address before choosing a type: this is
   the second time this session that only `reg.py` caught a wrong symbol declaration.

`arm9 0x020178e8` produced a clean fact about pointer declarations. The unit is 4 bytes short unless
the counter pointer `(u16 *)(node + 8)` is materialised between the two link stores; declaring it at
the top of the function makes mwcc emit `add lr, r0, #8` immediately after the first load instead,
which is both the wrong slot and the wrong register, and forces a frame. So the declaration *site*
picks the materialisation *slot*, but not necessarily the one you want - the scheduler will still
hoist it. Late declaration folds the address into `[r0, #8]` entirely and loses the instruction. There
is no middle setting found for this one.

The three-copy 96B family (`ov003 0x02082220`, `ov003 0x020c806c`, `ov005 0x0205ac30`) remains
unsolved: all three arms assign the same `(0x1e, 0x3c)` pair and both retail compiler builds merge
them into 80B, so retail's source must have made the arms non-mergeable in a way not yet guessed.
When that lever is found it is worth three units immediately.

## 3.21 Branch layout: the rare arm belongs in an `else` (2026-11-22)

`arm9 0x02030828` (96B) matched after one layout fix. It dispatches on `(a1 & 0x3f) == 0x11`: on a
match it calls the installed callback at `data_0204c18c` (guarded for null), returns when that yields
0, and otherwise calls `func_02030808`; on no match it calls `func_020263cc(a1 & 0x3f, a1)`.

Written as an early-return guard -

    if ((a1 & 0x3f) != 0x11) { func_020263cc(...); return; }

- mwcc inverts the test and inlines the `func_020263cc` call as the fall-through, which leaves the
  whole unit 96 bytes but completely reordered. Written as the positive test with the other arm in an
  `else`, mwcc emits `bne` to an out-of-line else at the end of the function and falls through into the
  main body, which is exactly retail. So an early-return guard and a positive if/else are *not*
  interchangeable here even though they mean the same thing: the early-return form moves the rare arm
  to the front.

Two more parks with the same "mwcc caches what retail reloaded" signature seen in `0x020178e8`:
`arm9 0x02029704` is exact in size and body but schedules a single `mov ip, lr` in a different slot
between the two guard compares, and `arm9 0x0201c17c` caches the pooled address of `data_020475fc` in
r4 for all three calls where retail reloads the literal at each site.

## 3.22 Two codegen shapes this compiler will not produce from source (2026-11-22)

Working the 24-96B tail turned up two shapes that neither 1.2/b56 nor 1.2/sp2 will emit from any
spelling tried, so they are worth recording as compiler-side walls rather than source puzzles.

**Loop if-conversion.** `arm9 0x020269ec` is a word-copy loop whose test sits at the top with the
copy pair predicated on it (`cmp r1, ip`; `ldmlt r0!, {r2}`; `stmlt r1!, {r2}`; `blt`), so the loop
both skips when empty and re-tests without a separate exit branch - 24 bytes. Every `while`/`for`
spelling produces a pre-check branch and 32 bytes; every `do {} while` spelling produces exactly 24
bytes but with the test at the bottom, which is the un-rotated loop. `vprobe` shows b56 and sp2
byte-identical here, so this is not a version-selection problem: it is an if-conversion pass we cannot
trigger from source. It also explains the older predication parks - a predicated body in retail is not
necessarily a predicated C expression.

**Frame-less argument saves.** `arm9 0x0203b4e4` opens with `push {r0, r1, r2, ip, lr}` and closes
with the matching `pop`, keeping the callee's return pointer in r3 across the pop. Both compilers
choose `push {r4, r5, lr}` plus a 4-byte frame for every source spelling, 56 bytes against retail's
40. Pushing the incoming argument registers wholesale (with `ip` as the alignment filler) is a
different register-assignment strategy, not a different statement order.

Also confirmed again on `arm9 0x02037660`: a byte field read into a local lands in r1 for us and r2
for retail at identical size, the third instance of that register-name residue.

No new matches landed in this stretch; the parks are all size-exact or within one scheduling slot.

## 3.23 The if-conversion wall is systematic, not a one-off (2026-11-22)

`arm9 0x020208b0` (display-mode setter, 140B) is the second, cleaner instance of the shape recorded in
3.22. Its `mode == 0` arm is four instructions including two literal-pool loads, and mwcc turns the
whole arm into predicated instructions:

    cmp   r0, #0
    ldreq r1, [pc, #0x4c]
    ldreq r0, [r1]
    biceq r0, r0, #0x80000000
    streq r0, [r1]
    beq   <tail>

Retail emits `beq` to that arm, placed out of line after the `mode == 0x100` arm, and never predicates
it. Predicating pool loads is safe for mwcc because the addresses are constants, which is presumably
why the threshold is generous - but it means that any retail function whose small arm contains a
hardware-register read-modify-write is unreproducible from C here, because our compiler always sees a
predicable block. Seven spellings were tried on this unit (if/else-if, empty else, reversed arms, a
shared hardware pointer local, a `switch`, and a block pointer), and the switch is the only one that
branches - but it produces a two-level dispatch with an extra comparison block and a stray `b`, 144
bytes.

The lesson for matching: if retail shows a *branched* small arm on a hardware register, do not keep
reformulating. The arm is predicable by construction and our compilers will always predication it.
Pick units whose small arms end in a call or another branch instead.

Also confirmed on `arm9 0x02028920`: keeping the base pointer in the incoming r0 and materialising a
field address in lr is an allocation we cannot request; our best form is two instructions shorter of
work and therefore needs fewer registers than retail does.

## 3.24 Two call sites need two statements, not one `if/else` (2026-11-22)

`arm9 0x0201f3d0` (104B) is solved in shape but not in bytes, and getting the shape right produced a
lever worth writing down. The function opens by testing bit 1 of `*(s32 *)(a0 + 0x108)` and calls
`func_0201f38c(a0)` in two different situations: when the bit is clear, and when the bit is set but
`a1 == 0`. Retail has the call **inlined twice**, at two separate addresses.

Written as an `if/else` whose two arms both end in the same call -

    if (bit) { if (a1 == 0) { f(a0); return; } ... } else { f(a0); }

- mwcc tail-merges the two identical call-and-return sequences into a single call site and the unit
  comes out 8 bytes short. Written as two consecutive early-return statements -

    if (!bit) { f(a0); return; }
    if (a1 == 0) { f(a0); return; }

- mwcc keeps both call sites, which is retail. So the earlier rule "identical arms get merged, split
  them" is only half of it: the merge happens across an if/else *join*, and two early returns are
  separate blocks that are never joined. This is the mirror of the 3.21 finding, where converting an
  early-return guard into a positive if/else was what fixed the unit.

The 8 bytes still missing are pure materialisation, the same class as 3.4 and 3.20: retail keeps the
bit-test value in two registers (`ldr r3` then `lsl r1, r3`), which forces `mov r2, r1` to preserve
`a1` before the shift, and materialises the flags-field address in r2 for the read-modify-write. Ours
folds the address into `[r4, #0x108]` and does the whole bit test in one register, so both of retail's
instructions have nothing to do. Seven spellings of that field access, including `volatile` and the
doubled-expression form, all fold.

`arm9 0x02024af8` is confirmed as the second instance of the veneer class already noted for
`0x02033bd0`: `ldr r1, [pc]; bx r1` to a real function start. Every C form of the tail call emits
`ldr ip` instead, both compiler builds agree, and a literal-address cast stops being a tail call
altogether. Veners are not reachable from source.

Roster note: a skeleton-matching sweep (unmatched units whose mnemonic+condition sequence equals an
already-matched unit's) found only 7 hits across arm9/ov002/ov003, and 6 of them are misaligned
four-byte artefacts of `SoftReset`-like padding. The family-substitution route is exhausted for the
unmatched set: the remaining units have no matched sibling.


## 3.25 Materialisation corpus: 42 matched units that DO keep `add rX, base, #K` alive (2026-11-22)

The recurring residue on the small-function tail is "retail materialises `add rX, base, #K` and
addresses everything through `rX`, we fold it into `[base, #K]`". Guessing at sources for it has
failed six times, so instead of guessing, here is the set of *already-matched* units that exhibit the
shape. Any future candidate with this residue should be diffed against the nearest member of this
table rather than re-derived from scratch.

Found by scanning every matched unit's retail bytes for `add rD, rN, #imm` (imm in 1..0xff, no
rotate) followed within four instructions by a load or store addressed off `rD`:

| unit | size | source |
|---|---|---|
| `arm9 0x2006be0` | 48 | `src/arm9/func_02006be0.c` |
| `arm9 0x200e8f4` | 52 | `src/arm9/func_0200e8f4.c` |
| `arm9 0x200e99c` | 48 | `src/arm9/func_0200e99c.c` |
| `arm9 0x2010280` | 56 | `src/arm9/func_02010280.c` |
| `arm9 0x20102f0` | 56 | `src/arm9/func_020102f0.c` |
| `arm9 0x201038c` | 52 | `src/arm9/func_0201038c.c` |
| `arm9 0x2015524` | 52 | `src/arm9/func_02015524.c` |
| `arm9 0x2018318` | 24 | `src/arm9/func_02018318.c` |
| `arm9 0x201c9e0` | 56 | `src/arm9/func_0201c9e0.c` |
| `arm9 0x201fc1c` | 32 | `src/arm9/func_0201fc1c.c` |
| `arm9 0x20221b8` | 108 | `src/arm9/func_020221b8.c` |
| `arm9 0x20279e0` | 52 | `src/arm9/func_020279e0.c` |
| `arm9 0x20289f4` | 68 | `src/arm9/func_020289f4.c` |
| `arm9 0x2028c98` | 72 | `src/arm9/func_02028c98.c` |
| `arm9 0x20298e4` | 76 | `src/arm9/func_020298e4.c` |
| `ov002 0x204c7e8` | 32 | `src/ov002/func_ov002_0204c7e8.c` |
| `ov002 0x204c824` | 96 | `src/ov002/func_ov002_0204c824.c` |
| `ov002 0x204d09c` | 68 | `src/ov002/func_ov002_0204d09c.c` |
| `ov002 0x204d0e0` | 76 | `src/ov002/func_ov002_0204d0e0.c` |
| `ov002 0x204d12c` | 76 | `src/ov002/func_ov002_0204d12c.c` |
| `ov002 0x204d1e8` | 68 | `src/ov002/func_ov002_0204d1e8.c` |
| `ov002 0x204d22c` | 68 | `src/ov002/func_ov002_0204d22c.c` |
| `ov002 0x204d270` | 68 | `src/ov002/func_ov002_0204d270.c` |
| `ov002 0x204d2b4` | 68 | `src/ov002/func_ov002_0204d2b4.c` |
| `ov002 0x204f898` | 64 | `src/ov002/func_ov002_0204f898.c` |
| `ov002 0x2060ca0` | 40 | `src/ov002/func_ov002_02060ca0.c` |
| `ov002 0x2062b48` | 32 | `src/ov002/func_ov002_02062b48.c` |
| `ov002 0x2062b68` | 36 | `src/ov002/func_ov002_02062b68.c` |
| `ov002 0x20633e0` | 84 | `src/ov002/func_ov002_020633e0.c` |
| `ov002 0x2063434` | 88 | `src/ov002/func_ov002_02063434.c` |
| `ov002 0x20636b4` | 108 | `src/ov002/func_ov002_020636b4.c` |
| `ov002 0x2063820` | 84 | `src/ov002/func_ov002_02063820.c` |
| `ov002 0x2063bb8` | 164 | `src/ov002/func_ov002_02063bb8.c` |
| `ov002 0x2063c5c` | 84 | `src/ov002/func_ov002_02063c5c.c` |
| `ov002 0x209b878` | 116 | `src/ov002/func_ov002_0209b878.c` |
| `ov002 0x209e44c` | 72 | `src/ov002/func_ov002_0209e44c.c` |
| `ov002 0x20a825c` | 104 | `src/ov002/func_ov002_020a825c.c` |
| `ov002 0x20ace70` | 72 | `src/ov002/func_ov002_020ace70.c` |

Four source shapes account for the small members, and they differ in ways that matter:

- `src/arm9/func_02018318.c` - `u32 *q = (u32 *)(p + 0x24); *q = *(u32 *)(p + 0x18); q[2] = 0;`
  The pointer is used at **two different offsets** (`[q]` and `[q, #8]`), which is what makes the
  base worth materialising.
- `src/arm9/func_0201fc1c.c` - struct members `s->a` and `s->b`, i.e. `[base, #8]` and
  `[base, #0xc]` through one struct pointer. Same two-offset rule, expressed as member access.
- `src/ov002/func_ov002_0204c7e8.c` - `u16 *q = (u16 *)p; u32 *r = (u32 *)p; q[3] = 0; q[2] = a1;
  r += 6; r[0] = r[0] | 2;` The second pointer is **advanced** (`r += 6`) before a single `r[0]`
  use, and the u16 stores already consumed the plain `p` addressing.
- `src/arm9/func_02015524.c` - `s32 *q = (s32 *)(p + 0xc); *q = *q + 1;` plus a *separate* full
  re-derivation of the same address in the following `if`. Two independent expressions for the
  same address is the extreme case.

The negative result that motivated this: on `arm9 0x0201f3d0` the field at 0x108 is read-modify-
written twice through one base and writing it as `u32 *p = (u32 *)(a0 + 0x108); *p |= 8;`, as
`u32 *r = (u32 *)a0; r += 0x42; r[0] = r[0] | 8;`, as a struct member, as a signed bitfield, with
`volatile`, with the value read into a local first, and as the doubled `x = x | 8` expression all
fold to `[r4, #0x108]`. Single-offset accesses do not materialise; only a second offset does.

## 3.26 The two-offset rule pays: two units first try (2026-11-22)

Acting on 3.25 instead of guessing produced two matches with no iteration on the source shape beyond
reading the disassembly.

**`ov002 0x020730ac` (136B)** allocates a record through `func_ov002_02077974` (passing
`func_ov002_02072db0` as the fifth argument's table, `0x14` as the size) and then fills four words of
it. The mark of the shape is `q = (u32 *)(p + 0x10)` with `q[1]`..`q[4]`: four different offsets
through one base is exactly what makes mwcc keep the `add r4, r5, #0x10` alive, and it came out
byte-identical first try. Two further details were needed:

- the null test must be a **positive** `if (p != 0) { ... } return p;` - the early-return form
  if-converts the whole body into `addeq/popeq/bxeq`, the wall from 3.22/3.23;
- the two arguments to `func_ov002_0204f70c` are `(s16)(*(u32 *)(a1 + 0x60) >> 8)` and
  `(s16)(*(u32 *)(a1 + 0x5c) >> 8)`, i.e. 16-bit signed fields packed at bit 8 of a word. Writing
  `(s8)(v >> 8)` gives three instructions (`lsr #8`, `lsl #24`, `asr #24`) where retail has two
  (`lsl #8`, `asr #16`), so the width is 16, not 8 - a good tell for how wide a packed field is.

**`ov002 0x02077b9c` (152B)** clears three sub-arrays of one record:
`*(u16 *)(p + i * 0x50 + 0x264)` for `i < 10`, `*(u8 *)(p + i + 0x584)` for `i < 7`, and
`*(u32 *)(p + i * 8 + 0x58c)` for `i < 4`, over `p = *(u8 **)data_ov002_02102070 + 0x1e6c`.
Matched first try. The `u16` loop counters are what produce the `lsl #0x10 / lsr #0x10` pair, per the
playbook, and the `mla` for the 0x50 stride follows from `i * 0x50` written as a multiply.

Both were installed through `reg.py` and the digest gate; `t10 --check ov002` reports
`389 entries, 389 sources, dangling=0, unlinked=0, dupes=0 -> ok`.

Also examined and parked: `arm9 0x020344b4` is a `memset` implementation (alignment prologue, byte
replication word, eight-word unrolled loop, 4-byte and byte tails) - library code, not writable from a
plausible C function here. `arm9 0x0200fcac` (148B) is a score accumulator: it adds the delta to fields
at +0xc and +8 of `*(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x154)`, then clamps +8 into
[0xc4653601, 0x3b9ac9ff] and returns it. Marking the chain `volatile` at both levels reproduces
retail's five independent re-reads, but retail also materialises `add r2, r1, #0xc` for a
single-offset read-modify-write, which is the one place the 3.25 rule does not explain; ours is 128B.

## 3.27 Struct copies want word members, and a byte store of -1 wants an s8 pointer (2026-11-22)

**`ov008 0x0204dd80` (152B) matched** - the third unit from the two-offset queue. It copies a 36-byte
table into a stack buffer, calls `func_020101d0` on each of the nine entries, records the index and a
zero byte for each hit into `p + 0xf`, then writes the hit count at `p[0xb]` and three trailing bytes.

Three things were needed and each is reusable:

- **The buffer struct must have word members.** `typedef struct { u8 b[0x24]; } Blk;` compiled the
  36-byte assignment into an 18-iteration `ldrb`/`strb` loop. `typedef struct { u32 w[9]; } Blk;`
  compiles it to retail's `ldm ip!, {r0-r3}` twice plus a single word, which is the shape to look for
  whenever retail copies a buffer in one go.
- **A byte store of -1 needs an `s8 *`.** `p[0xe] = -1;`, `~0`, `0xffffffff`, `(u8)-1` and a named
  `s32 v = -1` all compile to `mov r0, #0xff`; retail has `mvn r0, #0`. Storing through a cast
  pointer with a signed element type - `((s8 *)p)[0xe] = -1;` - produces the `mvn`. The store width is
  the same either way; only the constant materialisation differs, and the signed element type is what
  keeps it 32-bit.
- The tail loop's `n++` and `q += 2` only happen inside the hit arm, so the increment and the pointer
  advance are both predicated (`addne r4, r4, #1` / `addne sb, sb, #2`), which the plain
  `if (hit) { ... }` form gives for free.

Verified through `t10 --check ov008` (`26 entries, 26 sources, dangling=0, unlinked=0, dupes=0 -> ok`)
and the digest gate. `reg.py` initially reported a miss on the version of the source that still had
`p[0xe] = -1;`; the ROM diff was a single word (`e3a000ff` against retail's `e3e00000`), which is how
the `s8` requirement was found.

Remaining from the two-offset queue: `ov005 0x0204f8a0` and `ov009 0x0204eb14` are the same 224-byte
hardware state writer in two overlays (mask/or/shift combinations into 0x0400xxxx-style registers plus
a `mul` by -1) - a 2-for-1 if the constants can be pinned down.

## 3.28 Materialisation, third instance: two expressions for one address (2026-11-22)

No matches this pass, but the exception to the two-offset rule now has three instances and they agree
with each other.

`arm9 0x02014f68` (192/196) is one instruction short: retail computes `add r1, r2, #0x22` and does the
u16 read-modify-write through `[r1]`, while the same field is *also* read as `[r2, #0x22]` two
instructions later. `ov005 0x0204f8a0` shows it twice - the fields at B+0x68 and B+0x64 are reached
both as `[r3, #0x68]` (r3 = o + 0x6a00) and as `[r6]` / `[lr]` from *separate* pool constants - and
`arm9 0x0200fcac` shows it for a lone RMW.

So the rule from 3.25 needs restating: a base is materialised when the compiler sees **more than one
distinct address expression** for the same location, and "two different offsets through one pointer"
is only the case the corpus happened to be full of. Two spellings of the same address (`p->f` and
`*(T *)((u8 *)base + K)`) also do it, because they are separate trees and are not CSE'd against each
other. Our compiler CSEs them, which is why the address folds.

That immediately suggests the way to finish these units: write the second access through a *different
type or path* than the first, so the two address computations cannot be merged - a `volatile` read
keeps the second load (verified: 188B without it, 192B with it) but not the second address. In
`0x02014f68` the two accesses differ only in form (`*slot` against `*(u16 *)(O + 0x22)`), and mwcc
still folds both to `[r2, #0x22]`.

Also decoded in full and parked: `ov005 0x0204f8a0` / `ov009 0x0204eb14`, the 224-byte twin hardware
state writers, best 200/224 - the pseudocode and every constant are in `walls.txt`, so the pair is a
finishing job rather than an analysis job.

## 3.29 ov002 0x020766c4: signedness fixed the guards, the frame is the wall (2026-11-22)

`ov002 0x020766c4` (216 B, record creator) is the closest miss so far at 220/216, and two of its
levers transferred cleanly.

**Signedness is written into the branch condition.** The first guard is `cmp a0, [data+0x38]` with
`addge` / `bxge`, the second `cmp r0, #0x1000000` with `addgt`. With `u32 a0` and a `u32`-returning
callee we get `addhs`/`addhi`; declaring `s32 a0` and `extern s32 func_02034148(s32 a0)` gives
`addge`/`addgt` exactly, and it also settles the register assignment: a0 lands in sb and a1/a2/a3 in
r8/r7/r6, which is retail's layout. So the comparison opcode tells you the parameter's signedness
before you ever look at the data.

**Aggregate locals cost a frame word.** Retail is `push {r4,r5,r6,r7,r8,sb,lr}` with `sub sp, #0xc`
and three `s16` outputs at sp+4/+6/+8. `s16 t[3]`, `s16 t[4]`, `s16 t[5]` and `struct { s16 a, b, c; }`
all give a 0x10 frame (224 B) with correct code; three separate `s16` scalars give a 0x8 frame
(216 B, size-exact) but mwcc then folds `t1 - t2` into `sub r1, r5, r5` with r5 never allocated and
spills `p` instead of keeping it in r5. `s16 t[2]` plus a trailing `s16` gives 220 B with the loads
correct. mwcc reuses the word it stores as the 5th argument of the allocator call for the locals, so
the 0xc frame retail wants is not reachable from any of those spellings.

## 3.30 Two levers that transferred, and the same wall twice (2026-11-22)

`ov003 0x020d77b4` went 172 -> 196 of 204 and `ov005 0x0204f8a0` 200 -> 212 of 224, both without a
match. Two of the levers are worth keeping.

**A pointer local used at a small offset makes the RMW re-read.** `if (s[0x43] < 0x10) t[0] = t[0] + 1;`
with `t = (u16 *)(base + 0x8986)` emits a *second* load inside the predicate, which is what retail
does; writing the same address as `*(u16 *)(base + 0x8986)` on both sides reuses the comparison's
value and comes out four bytes short.

**Multiplying by `-1u` or `0xffffffff` gets `mvn` + `mul`.** `x * -1` gives `rsb r0, x, #0`; spelling
the constant unsigned gives retail's `mvn r1, #0` / `mul r0, r5, r1`.

The wall is the same one from 3.28, now with a sharper shape: retail materialises an address *inside*
a predicated block, and mwcc will not. For `ov003 0x020d77b4` the choice is explicit - assign the two
pointer locals before the call and they are materialised early with `add` (196 B), assign them inside
the block and mwcc folds them back onto the other pointer as `[r5, #0x86]` (180 B). Retail's third
option, `ldrlo r0, [pc]` + `addlo r1, r4, r0` inside the predicate, needs mwcc to keep the two
address computations apart; it unifies them through the shared base register instead.

## 3.31 What the ready set looks like now (2026-11-22)

Swept `ready.py`'s free (frameless, call-free) arm9 units instead of grinding the four blocked ones.
Four candidates, no match, but the failure classes are now distinct and each is cheap to recognise:

- **Unused frame.** `arm9 0x020248c8` produces retail's 14 instructions byte for byte and still misses:
  retail wraps them in `stmdb sp!, {lr}` + `sub sp, #4` / `add sp, #4` + `ldm sp!, {lr}` with no stack
  reference anywhere in the body. Forcing a frame with a struct local also changes the schedule, so the
  two cannot be had together. Both compiler builds give the frameless 56 bytes.
- **Address materialisation, again.** `arm9 0x020376c4` is 80/80 with a single differing instruction:
  retail's `add r1, r0, #2` for a lone `s16` field. Nine spellings, including volatile reads, pointer
  locals at two scopes, and two expressions for one address.
- **Chunking.** `arm9 0x02026ac8` is a 64-byte copy; retail uses 3-word ldm/stm with the parameters as
  running pointers, mwcc always picks 4-word chunks.
- **Not C at all.** `arm9 0x02024b04` needs `clz`; no intrinsic spelling survives in this toolchain
  (`__cntlzw` and friends all become external calls), so it is an assembly unit. Likewise the
  `mrs`/`msr` and `mrc`/`mcr p15` units at `0x02033c1c` and `0x02033c58`.

The edit loop is finding real C but the compiler refuses two specific peephole decisions: rematerialise
into a conditional block (3.30) and keep an unused frame (here). Both are visible in the first few
instructions, so triage is now a five-second check rather than a full attempt.

## 3.32 Match: arm9 0x0201f21c, and the "unused frame" was stack alignment (2026-11-22)

`arm9 0x0201f21c` (80 B) matches, and it explains the frame that blocked `arm9 0x020248c8` last pass.

```c
typedef struct T T;
struct T { u8 pad[0x118]; s32 f118; };
void func_0201f21c(T *t)
{
    if (t->f118 == 0) return;
    t->f118 -= 1;
    if (t->f118 != 0) return;
    func_0201cdb8(t);
}
```

Under the default 1.2/sp2 this comes out at 72 bytes; under **1.2/b56 it is byte-exact at 80**, and the
eight missing bytes are `sub sp, #4` / `add sp, #4` around a `push {lr}` that needs them: b56 aligns the
stack to 8 bytes across a call (AAPCS) and sp2 does not. Retail does align, so the unit needs
`src/arm9/func_0201f21c.c = 1.2/b56` in `config/ATOJ/mwcc_versions.txt`, and the full ROM digest still
matches with that override in place. `reg.py` takes the version as a sixth argument, so the install is
one command: `reg.py arm9 func_0201f21c 0x0201f21c 80 cand.c 1.2/b56`.

Two lessons worth the four turns they cost:

1. **A 4-byte frame with no stack references is not an unused frame.** Check whether it is the second
   half of an 8-byte AAPCS alignment; if it is, the unit is a b56 candidate, not a C-shape problem.
2. **Probe both compilers before writing the wall note.** `vprobe.py` answers in one run, and the
   version tag is a documented, digest-gated lever.

Re-probed and still short under both builds: `ov003 0x020d77b4` (196), `ov005 0x0204f8a0` (212),
`arm9 0x020376c4` (80 under sp2 with the materialisation folded, 84 under b56 with a trailing `bx lr`),
`arm9 0x020248c8` (56 under both; retail aligns without a call in sight, so the alignment theory does
not rescue that one on its own).

## 3.33 b56 sweep of the frame-shaped walls (2026-11-22)

Following 3.32, every walls entry whose residue was a bare `stmdb sp!,{lr}` + `sub sp, #4` pair with no
stack access was re-probed under 1.2/b56. Three improved, none matched:

| unit | sp2 | b56 | residue under b56 |
|---|---|---|---|
| `arm9 0x02017954` | 44/64 | **64/64** | `off` in r3 not ip, zero in r2 not r3 |
| `arm9 0x020208b0` | 136/140 | **140/140** | 3-register rotation: retail `mvn ip, r0` + flags in r2, we get `mvn r3, r0` + flags in ip |
| `arm9 0x02026aa4` | 84/36 | 84 | register moves plus frame, unchanged |
| `arm9 0x0203684c` | 48/40 | 48 | spill shape unchanged |
| `arm9 0x0202612c` | 28/20 | 28 | frame in the wrong direction |

So the b56 hypothesis is confirmed as a real class: the alignment frame appears (02017954 goes from 20
bytes missing to exact) but does not by itself fix the register allocation. Both survivors are now
"one register swap from a match", which is the dead end AGENTS.md tells you to record rather than
chase, so that is what `walls.txt` says for them.

## 3.34 Moving up in size: two near-misses that are both scratch-register ties (2026-11-22)

Started on `ready.py`'s mid-size, call-using units as planned. Both new candidates land on the same
class of residue, and neither is worth chasing per AGENTS.md.

`arm9 0x0201f26c` (68 B) reaches **68/68 under 1.2/b56** with the whole body in place - the nested
`if (f118 == 0) { if (callee(t) == 0) return 0; } f118 += 1; return 1;` and both builds keep the
argument in r0 the way retail does. The single diff is which scratch register holds the loaded `f118`
(r3 in retail, r1 in ours). Five formulations, including a `s32 v` local that made it worse (72 B).

`arm9 0x0202cff8` (84 B) is 88: the two guards, the `bic`/`str` RMW and the callee all line up, but
retail materialises `add r1, r2, #0x34` off a global kept in r2, while mwcc reloads the literal pool
for the second reference.

Worth recording as a rule of thumb: **a `ldr rX` / `cmp rX` pair where X is a scratch register is not a
source-shape signal.** Every attempt either changes the size or moves the value between r1 and r3; the
body is right and the allocator disagrees.

## 3.35 Two sibling idiom units: a flag-gated callback registry (2026-11-22)

Two of the `ready.py` units turned out to be the same idiom kernel, and one of them matched first try
once the operand order was fixed.

`arm9 0x0201bef4` (84 B) is **matched under 1.2/b56** with:

```c
int func_0201bef4(u32 mask)
{
    if (mask == 0) return 1;
    if (mask & data_020475ec) return 0;
    func_02027164(mask, 0);
    data_020475ec |= mask;
    return 1;
}
```

The first candidate wrote `data_020475ec & mask`, which gives `ands r1, r1, r4`; retail has
`ands r1, r4, r1`, i.e. **mwcc keeps the source operand order in a commutative `&`**. That single
swap was the whole difference, and it is the third time this log has recorded operand order mattering
in a two-register ALU instruction (§3.11 `add`/`sub` operands, §3.24 the `ldrh` form). Both builds
produced the same 84 bytes, so no version probe was needed; `reg.py` and `t10 --check arm9` agree.

For the record, the `movs r4, r0` / `moveq r0, #1` prologue means the zero test is on the argument
while the value stays live in r4, and the callee receives `(mask, 0)` with r0 untouched - a plain
two-argument call, not a saved-and-restored register.

`arm9 0x0202c36c` (84 B) is the sibling: check a slot, clear it, save the callback, clear it, then call
it. The instruction sequence and size land exactly (84 B, `stmdb sp!, {lr}` + `sub sp, #4`, `blx ip`),
but mwcc puts the global base in r1 where retail has it in r2, and that swap cascades into the `a`
temp and the zero constant (`ldr r1,[pc]` / `ldr r2,[r1]` / `movne r2,#0` vs retail
`ldr r2,[pc]` / `ldr r1,[r2]` / `movne r3,#0`). Spelling the global as `G *g` and using `g->a`,
`g->cb`, `g->c` is right in structure and shrinks to 80 B because mwcc drops the hoisted `g->c` load
out of the pivot; the raw `u8 *` + explicit offsets keeps the size. Tried: raw offsets with a `cb`
local, struct pointer with `cb` local, struct pointer with `a`/`cb` locals, `if (g->cb) { ... }`
block, and the raw form with `a` hoisted. It is parked in `walls.txt` as one register swap.

## 3.36 Three more units: a guard-flag setter, a buffered writer, and a signed bitmap test

Three small units fell out of the same `ready.py` batch, two of them first try.

`arm9 0x02015fe8` (72 B) is **matched under 1.2/b56**:

```c
u32 func_02015fe8(void)
{
    if (data_02042b40 & 0x8000) return 1;
    data_02042b40 |= 0x8000;
    func_02025c94(&data_02042b40, 0x18);
    return func_02015fa0();
}
```

The interesting part is the tail: the last statement is `return func_02015fa0();` and mwcc passes the
first callee's return value straight through in r0 without touching it, which is why the retail
`bx lr` is preceded by nothing at all. Only b56 was tried because the 8-byte alignment frame
(`stmdb sp!, {lr}` + `sub sp, #4`) is its signature.

`arm9 0x02034608` (84 B) is **matched under 1.2/b56** on the first candidate:

```c
int func_02034608(S *s, u32 src, u32 n)
{
    u32 cnt = n;
    if (s->used + n > s->cap) cnt = s->cap - s->used;
    func_02034494(s->buf + s->used, src, cnt);
    s->used += cnt;
    return 1;
}
```

with `struct S { u8 *buf; u32 cap; u32 used; }`. The clamped count `cnt` stays live across the call in
r4 and `subhi r4, r2, r3` is the ternary-free `if`, and `s->used += cnt` re-reads the field from
memory (`ldr r1, [r2]` before the `str`) exactly like the RMW pattern already recorded in §3.30.

`arm9 0x020124e0` (68 B) is a signed bitmap test - `return tab[idx / 8] & (1u << (idx % 8));` after a
null check on the table returned by `func_0200faa8()`. It reaches **68/68 with identical
instructions** but retail emits `add r1, r2, r1, ror #29` *before* `ldrb r0, [r0, r3, asr #3]` while
mwcc emits the load first; every spelling tried keeps the two swapped. Worth noting for the log:
`idx % 32` produced `lsl #27`/`ror #27` - the modulus is legible in the shift constant, so the
retail `#29` proved the source is `% 8` and not `% 32`. That got the last constant right, leaving only
the schedule residue. Parked in `walls.txt`.

## 3.37 Two more parks from the same batch (2026-11-22)

`arm9 0x020266c0` (80 B) is a DMA channel CR clear: `p[ch * 6 + 5] &= ~0x3a00` then `&= ~0x8000` on
`(volatile u16 *)0x040000b0`, between `func_02026350()` / `func_02026364()` lock calls. The index
arithmetic and both `bic`/`strh` pairs match, but retail is 80 B against our 72 B because it keeps
two *discarded* `ldrh r1, [r2]` reads after the last store (volatile reads are never deleted, so the
retail source evidently contains two reads whose values are unused), and it also keeps the constant 6
in r1 with the accumulator in r2 where mwcc chooses r0/r1. Padding with invented `(void)` reads to
buy eight bytes is exactly the kind of shape-forcing AGENTS.md forbids, so it is parked.

`arm9 0x0202cfa4` (84 B) is the main-thread loop around the same lock pair with a dispatch call
through `*(u32 *)(g + 0x30)`. Four live values (frame pointer, the 0 argument, `g + 0x3c`, the saved
lock token) have to share r4-r7; retail and mwcc agree on the set but not on the assignment, and the
body is 8 bytes long. Parked as a register-allocation tie.

Also noted while probing: `vu16` does not exist in `include/types.h` (only the plain scalar typedefs),
so `volatile u16 *` is the spelling to use for hardware-register pointers.

## 3.38 Four units from the ready queue, all 1.2/b56 (2026-11-22)

Back on `ready.py` after the documentation pass; four of the five candidates in this batch matched,
and each one turned on a single structural detail rather than a register.

`arm9 0x0203b640` (88 B) - deferred-callback drain loop. The shape that matches is a `while` whose
body reloads the global at the bottom, not a `do`/`while` that reassigns inside it:

```c
N *n = data_0204c790;
while (n != 0) { data_0204c790 = n->next; n->cb(n->arg, -1); n = data_0204c790; }
```

The initial load therefore feeds the **first** iteration from a register and only the bottom of the
loop re-reads memory (`ldr r2, [r4]` / `cmp` / `bne`), which is exactly retail. A `do { ... } while`
with the same statements produced a second load at the top and 92 B. The `-1` argument is loop
invariant, so mwcc hoists `mvn r5, #0` and copies it into r1 each iteration.

`arm9 0x0202c458` (132 B) - BCD nibble decode (validate eight nibbles, then accumulate base 10).
Both loops and the `mul`/`mla` pair were right on the first candidate; the residue was the *position
of the zero accumulator*: retail has `mov r5, #0 / mov r3, r5 / mov r2, r5`, i.e. the result variable
is zeroed **before** the validation loop and its register then seeds both counters. Writing
`u32 r = 0;` as a declaration ahead of the validation loop (instead of after it) removes the extra
`mov` and matches. Same lesson as §2.19 (a value live across a call has to be written as one), one
level down: a value live across a *loop* has to be declared where retail's copy lives.

`arm9 0x02020240` (116 B) - DISPCNT writer with a conditional mode and a mirror flag:

```c
u32 disp = *(volatile u32 *)0x04000000;
u16 flag = data_0204236c;
data_02048c70 = (u16)a;
if (flag == 0) a = 0;
*(volatile u32 *)0x04000000 = (disp & 0xfff0fff0) | (a << 16) | b | (c << 3);
if (data_02048c70 == 0) data_0204236c = 0;
```

The store to the mirror happens **before** the flag test, so retail stores the raw parameter and only
then zeroes the register the mode field uses (`strh r0, [r3]` then `moveq r0, #0`). Putting the store
after the `if` reads naturally and is one instruction shorter (112 B).

`arm9 0x02021c70` (116 B) - packed command-word builder with nine parameters. First candidate:

```c
*(u32 *)s[0] = 0x2a;
*(u32 *)s[1] = (i >> 3) | (b << 26) | (c << 30) | (d << 20) | (e << 23) | (f << 16) | (g << 18) | (h << 29);
s[0] = s[1] + 4;
s[1] = s[0] + 4;
```

The OR chain's operand order and the pointer advance through `s[1]` before `s[0]` both come straight
out of writing it in source order; the fifth through ninth arguments arrive at `[sp, #8]` upward
because the frame is the usual 8-byte alignment pair.

The fifth candidate, `arm9 0x02026a04` (76 B), is a 256-bit block fill that keeps matching retail's
`stmlt` block until the count loop; it is still open and is not parked.

## 3.39 Does everything need to be `1.2/b56`? (2026-11-22)

Prompted by the release date: the game shipped in Japan on **2004-12-30**, but our `1.2/sp2` is
build 79, `Runtime Built: Aug 11 2005` - seven months after the cart was on shelves. `1.2/b56` is
build 56 of `Sep 16 2004` (NITRO V1.0), which does predate it.

Measured instead of argued. `python tools/configure.py ATOJ --mwcc-version 1.2/b56` points both the
compiler and the linker at b56 for every unit, then `ninja sha1`:

* `tennis_ATOJ.nds` **fails** the digest (`609bd9ff...` against `8d555a80...`), so b56 is not a
  drop-in replacement for the current configuration.
* but objdiff with that build reports only **two** functions below 100% out of 1,536:
  `func_02025c30` at 90.5% and `func_ov004_0205e274` at 92.9%. Everything else - 1,532 of the 1,534
  complete units - is byte-identical under both compilers.

The two exceptions turned out to be source-shape artefacts, not evidence about the compiler - and
finding that out is what overturned the conclusion below.

They are not evidence that retail used build 79 either:

* `func_02025c30` is an inline `asm { }` block (the `mcr p15, 0, r2, c7, c14, 2` cache clean-and-
  invalidate loop), so its bytes come from the assembler path and say nothing about C codegen.
* `func_ov004_0205e274` is a source-shape artefact: retail stores 0 to `p->0x180` and then *reloads*
  it to pass as the second argument. Written as `*q = 0; func(p + 0x184, *q);`, sp2 keeps the reload
  and b56 folds the argument to the constant. The original source almost certainly read the field
  through something the compiler could not see through; the current line simply happens to reproduce
  retail under a compiler that postdates the game.

Both were then solved, and solving them is what settled the question:

* `func_02025c30`, written as `asm { }` **inside** a C function, is ordinary IR to mwcc: b56's
  optimiser folds `add r1, r1, #0x40000000` + `cmp r1, #0` into `adds` + `bne`, four bytes short of
  retail's 44. Written as function-level `asm void func_02025c30(void) { ... bx lr }` both b56 and
  sp2 emit the text verbatim, and `reg.py arm9 func_02025c30 0x02025c30 44 <file> 1.2/b56` prints
  **MATCH**. The block was never a compiler fingerprint; the form of the assembly was ours.
* `func_ov004_0205e274`: retail stores 0 to `p->0x180` and then *reloads* it for the second
  argument. `*q = 0; func(p + 0x184, *q);` lets sp2 keep the reload and b56 forward the constant;
  writing the argument as `*(u32 *)(p + 0x180)` instead of through the cached `s32 *q` matches b56
  at 64 B with no `volatile` needed.

With both fixed, the decisive run: `python tools/configure.py ATOJ --mwcc-version 1.2/b56` then
`ninja sha1`

```
Target: 8d555a80db47278475ffdfdf808fe80dbd7c48cf; tennis_ATOJ.nds: 8d555a80db47278475ffdfdf808fe80dbd7c48cf
```

- all 1,536 units, one compiler, **zero per-file overrides**. `t10.py --check arm9` is clean
(`883 entries, dangling=0, unlinked=0, dupes=0`). b56 is retail's compiler.

That reframes the 72 overrides accumulated under the sp2 default: they were the price of the wrong
baseline, not evidence that retail mixed builds. `config/ATOJ/mwcc_versions.txt` is now header-only,
`tools/configure.py`'s `DEFAULT_MWCC_VERSION` is `1.2/b56`, and the AGENTS.md rule was rewritten to
state the measured fact (the digest, not the object diff, still decides).

**Correction worth recording.** This section first concluded the opposite: that the statement-level
`asm` block ruled b56 out, and that retail must be an unowned 2004 build such as build 68 sharing
sp2's behaviour on `asm`. The error was assuming `asm { }` is passed through unchanged - it is not,
b56 optimises it, and the 40-vs-44-byte difference was our own writing. The date argument survives
and now agrees with the digest: build 79 (`Runtime Built Aug 11 2005`) cannot have built a
2004-12-30 cartridge, build 56 (`Sep 16 2004`, NITRO V1.0) can and does.

Remaining caveat: the digest proves the bytes, not the provenance. Nintendo could in principle have
built SDK library objects with its own build or flags; nothing in `src/` shows it here, because b56
alone reproduces every installed unit, but a future unit that needs another version would be the
first evidence of it rather than a mystery to be parked.

Later that day the pin moved to the decrypted revision (`9eec3248...`) and the header
step in `tools/build_rom.py` was added; the hashes quoted in this section are the ones those runs
printed at the time, against the encrypted pin.

## 2.80 The arm9 ready queue is SDK code, and one more tie (2026-11-22)

`ready.py --limit 40 --max 200 arm9` lists 87 ready units, 11,868 bytes. Sampling four
of them says what the queue is *made of*, and it is not game code:

* `func_020261d0` (108 B) is a context switch: `mrs`/`msr cpsr_fsxc`, saves `sp` into a
  register, calls `func_02026090`, makes an indirect call through a pool pointer, calls
  `func_020260a0`, restores. No C source produces that sequence, it is the OS's own asm.
* `func_0202564c` (76 B) is an exception-frame install: `mrs r2, apsr`, `msr cpsr_c`,
  `str sp,[r1,#0x40]`, then a 15-register `stm r1, {r0-r8,sb,sl,fp,ip,sp,lr}` and a
  pc-relative return address stored at `+0x3c`. Same class.
* `func_0203aeb8` (12 B) and `func_02032b00` (48 B) are not functions at all: the first is
  `push {r4-r7,fp,ip,lr}` + `mov r4,#0` + `b` into code outside its own 12 bytes, the second
  pushes `{r0-r3}` **without `lr`** and ends `bx lr`, so it cannot return. Both are delinks
  boundaries that fall inside a larger routine with more than one entry point.

So "ready" in that roster means "all callees matched", not "matchable from C". The arm9
remainder is largely NitroSDK, and the overlays are where the game code is.

`autorun.py 40 96 arm9` on the same queue: 0 candidates, 54 bailed, 465 already tried.
The generator has nothing left to say about this module.

One tie worth remembering rather than retrying: `func_02017954` (64 B, ready, no calls).
Seven shapes agree on the frame, the two zero stores, the two pointer stores and the u16
increment, and differ only in which temporary lands where:

```
retail: ldrh ip,[r0,#0xa]  mov r3,#0  add r2,r0,#8  add lr,r1,ip  ...  ldrh r0,[r2]
ours:   ldrh r3,[r0,#0xa]  mov r2,#0  add lr,r0,#8  add ip,r1,r3  ...  ldrh r0,[lr]
```

i.e. retail allocates the count pointer and the zero before the load of `n`; mwcc here
allocates by first use and will not move `n` out of `r3`. Declaration order of
`count`/`zero`/`n`, a `u32 zero` variable, `u8 *t = q + n` versus the inline expression,
and `p->count++` versus `*count = *count + 1` all produce the same rotation. Taken with
`2.79`, the register-role class is the dominant residue in what is left, not the logic.

Where the bytes actually are, from `famsurvey.py`: ov002 has 58 identical-skeleton groups,
including 13x756 B (9,828 B), 14x608 B (8,512 B), 10x688 B and 10x676 B. One matched
member plus constant substitution covers each family, so a single ov002 shape is worth
more than a dozen arm9 ties.

## 2.81 Five Thumb SWI stubs are byte-exact but cannot be installed (2026-11-22)

Scanning for unmatched units whose full mnemonic skeleton equals a matched unit's found
seven in arm9 and none in the overlays. Five of the seven are the same shape as the matched
`SoftReset`: a two-instruction Thumb BIOS stub, `swi N; bx lr`. Retail bytes read straight
out of the ROM, `swi` immediates from the low byte of the first halfword:

| unit | address | bytes | `swi` |
|---|---|---|---|
| `Halt` | `0x020040e6` | `06df 7047` | `0x06` |
| `Sqrt` | `0x02004146` | `0ddf 7047` | `0x0d` |
| `IsDebugger` | `0x02004336` | `0fdf 7047` | `0x0f` |
| `CpuSet` | `0x02004466` | `0bdf 7047` | `0x0b` |
| `GetCRC16` | `0x020044ea` | `0edf 7047` | `0x0e` |

Each one written as

```c
#include "types.h"

#pragma thumb on
void Halt(void)
{
    asm { swi 0x06 }
}
```

is byte-identical: `var3.py` prints `MATCH` for all five, and `fastmatch.compile_text`
returns exactly `06df 7047`. None of them can be installed, though — `reg.py`/`t10.try_unit`
print `miss`, and a manual install showed why:

* the arm9 entry point in the rebuilt header moves from `0x02004800` to `0x02004804`,
  i.e. the whole image shifts by four bytes;
* 218,294 bytes differ, starting in the header.

So the four bytes are not being carved out of the module blob when the unit is installed, and
the new section is appended instead. `src/arm9/SoftReset.c` is the same shape and *is*
installed, with a delinks entry

```
src/arm9/SoftReset.c:
    complete
    .text       start:0x0200406c end:0x02004070
```

so the entry format is not the problem — the difference is in how the delink/relink handles a
second four-byte hole in that Thumb region. This is a tooling issue to fix before those five
units (and `func_02024af8`, a twelve-byte pool jump) can land; the sources and bytes are
already known and should not be re-derived. The tree was reverted and `ninja sha1` printed
`OK` again before this was written down.

**Cause (same day).** The pad is alignment, not the carve. `Halt.o` — both the delinked one and a
freshly compiled one — has `.text` with `sh_addralign = 4`, and the target address `0x020040e6`
is only 2-aligned, so the linker inserts two zero bytes before the unit and everything after
moves. `SoftReset` escaped this only because `0x0200406c` happens to be 4-aligned. dsd will not
take a per-entry fix: adding `align:2` to the delinks entry is rejected with *"attribute
'align' should be omitted as it is inherited from this file's header"*.

That means retail did not have these stubs as separate objects — a 2-mod-4 function boundary can
only exist inside an object, so in the original several functions were compiled in one
translation unit. Two ways out, either of which needs the digest as the gate:

* co-locate the stub with the function that ends at its address, in one source file, remembering
  that mwcc emits functions in reverse source order; or
* find a way to emit the unit's `.text` with 2-byte alignment, which `mwccarm` does not do for a
  plain function.

Until one of those is chosen, the five stubs stay out even though they are byte-identical.

**Scope of the alignment wall.** Only nine functions in the whole ROM start at `addr % 4 == 2`
(38 bytes, all arm9, all Thumb BIOS wrappers): `Halt`, `Sqrt`, `RLUnCompReadNormalWrite8bit`,
`VBlankIntrWait`, `IsDebugger`, `LZ77UnCompReadByCallbackWrite16bit`, `CpuSet`, `GetCRC16` and one
more in `0x020045xx`. Zero matched units sit at such an address, which confirms the wall is real
and also that it is a 38-byte corner, not a systemic obstacle — re-tooling the linker for it is
not worth the risk to the 1,536 units that link today. `tools/match/walls.txt` now lists them
with the reason.

## 2.82 Some arm9 Thumb units need mwccarm 2.0, not 1.2/b56 (2026-11-22)

`func_0201fcc8` (`0x0201fcc8`, 28 B, `kind:function(thumb)`) is a nine-word fixed-point block
initialiser:

```c
#pragma thumb on
void func_0201fcc8(u32 *p, u32 a, u32 b)
{
    p[0] = 0x1000;  p[1] = 0;  p[2] = 0;  p[3] = 0;
    p[4] = b;       p[5] = a;  p[6] = 0;  p[7] = -a;  p[8] = b;
}
```

Under 1.2 (all six builds) the `0x1000` becomes a literal-pool load and the unit is 32 B:

```
0249 0360 7047 c046 00100000        ldr r3,[pc]; str; bx; pool
```

Every 2.0 build emits retail's form instead:

```
0123 1b03 0360 7047                 movs r3,#1; lsls r3,#0xc; str; bx
```

Installed with `reg.py ... 2.0/base`; the full ROM digest still matches, so this is a split
build artefact and not a flag problem: a `2.0/base` entry for one file in `mwcc_versions.txt` is
the same evidence standard as any other override. Primitive probe: compiling
`p[0] = 0x1000;` under 1.2/* yields `0149 0160 7047 c046 00100000`, under every 2.0/* and dsi/*
build it yields `0121 0903 0160 7047`. So when a Thumb unit is a couple of bytes short and has a
pool word that fits `1 << n`, try 2.0 before rewriting the source.

`tools/match/wallprobe.py` now probes `2.0/base` as well (543 previously parked bodies across
arm9 and the overlays: no wall converts, so the parked list is unaffected by the finding).

The `0x1000` constant is not the only thing 2.0 changes: the same function's two neighbours
(`0x0201fce4` 28 B, `0x0201fd00` 24 B) are byte-identical to their source under 2.0 except for
which of r2/r3 carries the zero and the negation, and `0x0201fd00` additionally wants `stm r0!`
stores where both compilers emit `str` for the array form. Both are still open and remain
unparked.

Tooling note: `var3.py` disassembles the retail side with the *module* mode (`arm9`), so a Thumb
unit's diff is printed as ARM nonsense even though the byte comparison itself is correct. The
`got`/`want` sizes and the byte compare are trustworthy; the instruction listing is not, for
`kind:function(thumb)` units in arm9. Use `udis.py`/capstone with `CS_MODE_THUMB` when reading
those.

## 2.83 Five arm9 units, and two bitfield-arity levers (2026-11-22)

A batch picked by adjacency to already-matched units produced five installs:
`func_0201f1bc` (96 B), `func_0201cb84` (96 B), `func_02025550` (104 B),
`func_02018c70` (108 B) and `func_0201f3d0` (104 B). Three of them were first-try
transcriptions; the two bitfield units needed the levers below.

**`s32 x : 1` for an `asr` read (2.59, confirmed).** `func_0201cb84` tests a flag word
at `0x24` and `func_0201f3d0` one at `0x108`. Retail reads them as

```
ldr r0, [r4, #0x24]
lsl r0, r0, #0x1e
asrs r0, r0, #0x1f
```

Writing `*(u32 *)(a0 + 0x24) & 2` gives `tst`/`and` instead. Declaring the member
`s32 busy : 1` at bit 1 produces the shift-and-arithmetic-shift pair exactly; a `u32`
member produces `lsrs` and the sign of the test flips to `bne` on an unsigned compare.
For `func_0201f3d0` the same struct covers two fields (`f1` at bit 1, `f3` at bit 3),
and the set is a plain `orr r1, r1, #8`.

**Pass the field, not a literal, when retail omits the `mov` (new).** `func_0201f3d0`
calls `func_0201f38c` twice and retail passes `r0 = a0`, `r1 = <the asr result>`,
`r2 = a1`, having copied `a1` to `r2` at entry (`mov r2, r1`). Writing the obvious
two-argument call `func_0201f38c(a0, a1)` reproduces the entry copy but leaves the
function 4 B short, because neither call site needs `r1`. Writing a three-argument
call with a literal `func_0201f38c(a0, 0, a1)` matches everything up to those calls and
then inserts `mov r1, #0` before each, overshooting by 8 B. Only reading the field into
a variable and passing *it* - `s32 b = fl->f1; ... func_0201f38c(a0, b, a1);` - lands:
the `asr` result is already in `r1`, so both call sites are bare `bl`. The `mov r2, r1`
at entry is the tell that the callee takes three arguments and the second one is the
field just tested.

**`if (!p) return;` before a loop that reloads its bound.** `func_0201cae0` walks
`a0->count` entries and reloads `[r7, #0x48]` at the bottom of every iteration, so the
bound must be read through the pointer inside the loop condition rather than hoisted
into a local by hand; the compiler keeps the reload because the preceding call could
write it.

**Two identical loads of one global are sometimes the source.** `func_02025550`
(sorted list insert) only reproduced once `head` and `cur` were both initialised from
`*(u8 **)(data_02048d3c + 0xc)` as two separate loads. Sharing the value
(`u8 *cur = head;`) keeps retail's 104 B but leaves the loop in a different register
colour - `r3`/`lr` swapped - for a diff of 8. The two-load spelling gives retail's
`lr` for the cursor and `r3` for the saved head.

## 2.84 Copied argument registers as a search key, and a `var3` trap

The `mov rX, rY` tell from §2.83 is cheap to search for: disassemble the first few
instructions of every unmatched function and flag a copy between `r0`-`r3` before the
first call. Two hits, both real. `func_0202f448` uses `a2` twice - as the shift count in
`1 << a2` and as the fourth call argument - so mwcc parks it in `r3`. `func_0201c59c`
turns out to be a five-argument allocator call with a callback pointer
(`func_0201bfd8`) as its third argument.

`func_02025a54` and `func_02025b14` are the reference-count pair around `data_02048d3c + 8`:
retail duplicates the increment block in the two taken arms of `if (a == 0) ... else if
(a == g) ...`, and only the `if`/`else if` spelling reproduces that. Writing two
independent `if`s lets mwcc merge the arms and loses 24 B.

`func_020271d4` (flag-bit dispatch) only matches as `while (i < 8 && m != 0) { ... i++;
m >>= 1; }` with `i` declared before `m`; a `for` loop moves the `i < 8` test to the bottom
of the loop, and declaring `m` first swaps `r4`/`r5`.

The trap: both ref-count functions first reported `MATCH` from `var3.py` with the wrong
data symbol. `var3` compares instruction text, and an undefined symbol still prints as
`ldr r1, [pc, #..]`, so the pool word's *value* is never checked. `reg.py` caught it
immediately (`sha1` false, two bytes differing at the pool). Read the pool word out of the
image; only `reg.py` accepting is evidence.

## 2.85 Signed guards, redundant guards, and two constants into one address

`func_020123e0` opens `cmp r6, #0xc; movgt r0, #0` - `gt`, not `hi`. A range guard written
against a `u32` parameter emits `movhi`, so retail's parameter is `s32`; switching the
declaration is free and obvious only if you read the condition code.

`func_020190f8` showed the opposite: an explicit `if (n == 0) return;` in front of a
`for (i = 0; i < n; ...)` loop makes mwcc hoist *two* zero tests, because the loop's own
guard cannot reuse the truncation's flags once the statements are separate. Deleting the
redundant guard gives retail's shape exactly - the `lsrs r8, r1, #0x10` sets the flags and
the single `addeq` epilogue is the loop test.

`func_020235a0` writes two constants to one address from two conditions. Written as
`if (r == 0x20) A; else if (r == 0x40) B;` mwcc merges the stores and comes up 4 B short at
108 B. Written as `if (r != 0x20) { if (r == 0x40) B; return; } A;` it reproduces retail's
112 B, including the `0x20` case sitting out of line at the end.

`func_0201e5f8` needed the return type, not the body: the early exit is `moveq r0, #0`
followed by `popeq`, which no `void` function emits. Declaring the function `s32`, returning
`0`, and returning the callee's result gives retail's frame - and the trailing call stays
`bl` + epilogue rather than becoming a tail `b`.

## 2.86 Declaration order, shared failure exits, and stack arguments

`func_0202f118` is 372 B and differs from retail only in which callee-saved register holds
the `func_0202e2ec()` base and which holds a `u16` field. The whole fix is declaration order:
with `u8 *h = ...; s32 m = 1; s32 f; s32 r; u16 n;` mwcc colours the base `r6` and the field
`r4`; with `u16 n;` moved to the front of the list it produces retail's `r4` base and `r6`
field. No statement changed. `func_0202f28c` has the same shape but only one local competing
with four parameters, and every declaration order there leaves the two groups swapped, so it
is parked in `walls.txt`.

`func_0202ee48` needed the bounds checks written as one condition. Two separate
`if (sum > 0x200) return 6;` lines get if-converted into two four-instruction predicated
epilogues and land 12 B long; a single `if (c1 > 0x200 || c2 > 0x200) return 6;` puts one
shared failure block between the two tests and matches. The same function's second
`func_02025c78(x, *(u16 *)(p + 4))` call reuses the halfword that the `if` had just loaded,
which is what retail does - the second argument is that field, not the `0x40` used by the
first call.

`func_020303a0` showed a fifth argument hiding on the stack: `str r0, [sp]` immediately
before the `bl` is the fifth argument of `func_0202ffe4(p + 0x140, 8, 0xffff, 2, 1)`, not a
message-buffer store. Reading it as a local was what made the `str` schedule late and in
`ip` instead of `r0`.
## 2.87 One unit closed, five idioms parked

`func_02036650` (60 B, `strchr`-shaped) was the session's only new match. Its entry sequence
is `ldrb r2, [r0]` / `and r1, r1, #0xff` / `add r0, r0, #1`, and mwcc insists on fusing the
load and the increment into `ldrb r2, [r0], #1` whenever the source compares against the `u8`
parameter. Copying the parameter through a `u8` local (`u8 cc = (u8)c;`) makes the mask a
separate early statement, keeps the entry split, and matches; a `u32` parameter with
`(u8)c` at each use gives the same instructions in a different order (64 B).

Everything else surveyed this session is recorded in `walls.txt` with its measured residue:
the `~v`/`p[0]` `ip`-vs-`r3` swap in the 140 B `0x020208b0`/`0x0202093c` twins, mwcc's framed
`lr`/`ip` 16-byte struct copy against retail's unframed 12-byte `ldm`/`stm` chunks
(`0x02026a88`, `0x02026aa4`, `0x0202474c`), the four-byte stack realignment mwcc adds to
call-only bodies (`0x0202612c`, `0x0203b4b0`), the `pl`/`mi` threshold test plus unfused
`mov`/`lsr` in `0x02024b5c`, and the zero-register colouring of three stack arrays
(`ov003 0x020b6b60`, `0x020b732c`). `near.py --install` found no near twins in arm9 or any
overlay this session.

### 2.88 Small-unit survey: one 48B match, eleven new walls

Eleven fresh sub-72B units were decoded and attempted. `func_020255b8` closed at 48B once the
scanned table was declared as a struct field array instead of a `base + 0x14 + i * 4`
expression (the lever is in `mwccarm-codegen.md`); the other ten are now in `walls.txt` with
measured residue, plus the previously decoded ov005 220B four-family.

The recurring failure modes this batch, in order of frequency:

1. **Loop layout, not loop source.** Retail emits `cmp` / conditional-store / `blt` back to
   the `cmp` (`0x020048e8`, `0x020269ec`) or `b test` with the test at the bottom
   (`0x02032be8`); mwcc rotates the same `while` into an early-exit guard plus a bottom test,
   or emits `movs`/`bxeq` instead of the `b`. Four spellings each (while, for-with-increment,
   separate store and bump, do-while) reproduced mwcc's layout every time, so the rotation is
   not source-controllable here.
2. **Fused flags.** `while (n >= 4) n -= 4;` (`0x020263c0`) is `subs` / `bhs` in retail, which
   folds the borrow into the branch; mwcc only folds it for the do-while spelling, and that
   spelling is 4-8B too long.
3. **Scratch colouring.** r12/ip versus the next free caller-saved register decides
   `0x0202c430`, `0x020266c0`, `0x0202497c`, and the ip/lr swap that is the *only* residue in
   the four-copy ov005 220B family. `0x0202d158`/`0x0202d22c` are 212B-exact apart from one
   `mov r1, r6` and a base reuse.
4. **Literal-address reuse.** `0x0201fd50` needs three pool words with `#4` offsets from two
   bases; per-literal loads give 76B and a struct gives 52B with reordered stores.
5. **Aggregate copy strategy.** `0x02026ac8` (64B struct) and `0x0203b4e4` (argument
   preservation across a call): retail stays in the argument registers or 3-register chunks,
   mwcc frames and uses callee-saved registers.

Also attempted and parked earlier in the same pass: `0x0202497c` (three live values across a
call), `0x02032be8` (layout), `0x020266c0` (colouring).

### 2.89 Two more small units; four walls of the same two kinds

`func_020255b8` (48B) closed on the struct-field-array lever. `func_0202963c` (80B) closed
once the walk was written as `while (p != 0 && *(s32 *)p != v) p = *(u8 **)(p + 4);` - the
`&&` form makes mwcc emit retail's `b test` / body / test layout, where `while (p) { if (...) break; ... }`
produced a four-byte-longer straight-line version.

Four new walls, all of kinds already in the log: branch inversion with if-conversion
(`0x020202b4`), pool-versus-`add` materialisation of a base plus field pointer
(`0x0202cff8`), a scratch-register tie plus a folded-operand order (`0x020101f8`), and a
hoisted base address that mwcc rematerialises per use (`0x0202cfa4`).

Two negative results worth keeping:

- The ov005 `0x020614a4` family resists every spelling tried: `while`, `for`, separate
  pointer variables declared inside and outside the loop, a `u16` index into a `u16 *`, and
  an explicit `u16 z = 0`. All are 220B and all differ only by the ip/lr swap on the zero
  constant and the `base + i * 2` address.
- `0x0201fd50`, `0x02026ac8` and `0x0203b4e4` keep the residues recorded in `walls.txt`
  (per-literal pool loads, aggregate copy strategy, argument preservation in r4/r5).

### 2.90 List walkers: three units in the `0x0201c4b0`/`0x020181b0`/`0x0202cc90` band

`func_0201c4b0` (92B) closed once the traversal was written against a `next(list, prev)` style
API - `p = func_02017784(data, p);` at the bottom of the loop, with the same call feeding the
pre-test from `0` - instead of a `p = p->next` load. The call clobbers r1, so retail simply
leaves the previous pointer in it as the next call's second argument, which is not something a
pointer-chasing expression can express.

`func_020181b0` (88B) closed on a colouring refinement: declaring `u32 t; u8 *b; u8 *p;` and
assigning in statements rather than initialising in the declarations, which swaps r4/r5 into
retail's order. The `u32`/`u8 *` mix also needs an explicit `(u32)p` on the list-backpointer
store or the unit does not compile.

`func_0202cc90` (92B) is parked: it is the `0x0202d158` flag-wait tail without the argument
stores, and it hits the same `mov r1, r5` tie on the first flag load.

The ov005 `0x020614a4` four-family still resists: every form that lands on 220B keeps the
ip/lr swap on the zero constant and the `base + i * 2` address, so the tie is in the
allocator's temp order, not in the loop's source shape.

### 2.91 `func_02008178`: saturating table/counter lookup

96B, closed by two separate details. The table read is a byte offset with a `mul`
(`*(u32 *)(data_0203c0f4 + a0 * 12)`), not `((u32 *)data)[a0 * 3]`, which would scale an
index and pick the wrong addressing mode. The guard is a *signed* compare: `s32 v` with
`v >= 0x3b9ac9ff` gives `ldrge` / `bxge`, where `u32 v` gives `ldrhs` / `bxhs` and leaves the
unit one predicate away. The `if (n <= 1) return 0x7d0;` that the disassembly seems to show is
just the loop's pre-test falling through with `v` already in r0, so it must not be written.

Parked in the same band: `0x0202d9f4` (base rematerialised instead of held in r4 across a
call) and `0x0201c17c` (two-condition walk gets an extra test pair).

### 2.92 Three near-misses in the 100-112B band

Surveyed the 108-112B units next. `0x0202ad9c` (six-field copy to I/O registers) landed
size-exact at 108B on the first struct-copy attempt, `0x02029704` (string to packed lowercase)
at 100B on the third, and `0x02033014` stayed one instruction long. All three are parked in
`walls.txt` with their measured residue; none of them is a binding problem to solve later, they
are allocator/scheduler ties.

What the survey bought:

- `typedef struct { u32 a; u32 b; }` copies are the only spelling that produces retail's paired
  `str x,[rN]` / `str x,[rN,#4]` from two consecutive destinations. Eight separate literal-address
  stores give eight pool loads.
- A `u32` conversion temporary keeps `and #0xff` out of the case fold; a `u8` lvalue forces it.
- A trailing one-block `if` is always expanded to `popeq`/`bxeq`; retail's `beq` to a shared
  epilogue is not reachable from source at this compiler version.

`0x0202ad9c` is the interesting residue: identical instruction sequence, size-exact, with the
allocator rotating the whole four-register set (retail holds the pair temporaries in ip/r3 and
the destinations in r2/r1). Naming the destinations as locals does not lengthen their live
ranges because mwcc folds the constants back into first-use pool loads. The same rotation
appears on `0x020255e8` (100B, three instructions) and `0x02017880`/`0x020178e8`.

### 2.93 Three families closed on shape, none on colouring

`famsurvey.py` still reports five groups in arm9; four of them are now understood and parked.
All three families below reach retail's exact size and instruction sequence and differ only in
which registers the temporaries get, or in one materialisation:

- `0x02028920`/`0x02028958` (56B each): `*cur += p->f34` plus an indirect vtable call. Two
  independent loads come out one slot apart (mwcc: `*cur` then `p->f34`; retail the reverse) and
  the vtable entry gets r3 instead of a reused ip. Naming the callee pointer, splitting `d` into
  its own variable and reordering declarations all leave it.
- `0x020208b0`/`0x0202093c` (140B each): 140B exactly, mask/v18/v0 rotated through ip/r3/r2.
  Two real levers came out of this one (symbol base, `else`-arm if-conversion) and both moved the
  unit closer without closing it.
- `0x02023df8`/`0x02023e58` (96B each): 92B, because mwcc folds the 0x06600000 base into both
  `add` immediates instead of materialising it once in ip ahead of the branch.

The lesson for this band is that the remaining residue is the allocator, and the allocator reacts
to things the disassembly cannot show — symbol versus literal addressing is the newest example.

### 2.94 `true_fresh.txt` carries stale entries

Chasing `0x0200fbc0` and `0x0200fc18` (the increment-then-clamp pair over
`data_02042a68 + 0x10` -> `+0x154` -> `+2`/`+0x10`) produced a full `MATCH` on a rewrite that
swapped the literal for the data symbol and split the two chains into `p` and `y` locals — and
no change in the totals. The queue entries were stale: both units already had matching sources
in `src/arm9/`, and compiling the committed ones gives `MATCH` too, so the rewrite was cosmetic
churn and was reverted.

The queue is therefore not proof that a function is unmatched. Check
`src/<module>/func_<addr>.c` (or the objdiff report) before spending iterations on an address
that came out of `true_fresh.txt`; the report-derived `famsurvey.py` groups were accurate every
time this session.

### 2.95 A reliable unmatched list, and the packed-word lever

`tools/match/true_fresh.txt` was stale twice, so the queue used this session comes from
`symbols.txt` itself: every `kind:function(arm,...)` with no `src/<module>/<name>.c`, minus
anything already in `walls.txt` (match with `0x%08x`, not bare `hex()` — the walls file zero-pads).
That leaves 87 arm9 units in the 40-160B band.

First candidate out of it, `0x02027080`, produced the packed-word lever above and reached 88B
with only the two chains' interleaving left. Two related units in the same band:

- `0x02032aa8` (88B) and `0x02032b00` (48B) share the odd `ands r0, r1, #1` / `ldrhne` /
  `andne #0xff00` / `asrne #8` shape — an unaligned 16-bit read written as a macro. Both are
  unmatched and belong to one source shape.
- `0x02025698` (68B) is a context restore (`msr cpsr_c`, `ldm ... ^`); `0x02033e40` (88B) is a
  varargs trampoline with `ldm r2, {r3}` — both off-limits per the existing notes.

### 2.96 Keyed list removal matched at 0x0201779c, insert parked at 0x0201780c

`0x0201779c` (112B) is the removal for a doubly linked list whose node carries a u16 key at
+0xa used as the offset of its two links. The body came out right on the first spelling; the only
gap was the tail counter, which needs its address in a pointer (`u16 *cnt = (u16 *)(node + 8);`)
instead of an inline `*(u16 *)(node + 8)`. Installed, digest verified, pushed.

`0x0201780c` (116B) is the insert counterpart and is parked: the same source shape gives the
same size and instruction order, but the copied link and the destination address swap ip and lr
at 0x040-0x050, which is five words of the linked ROM. `reg.py` is the only thing that shows
this, since the object compares size-equal.

Also confirmed this session: `var3.py`'s diff must be read to the end. It prints `bl` target
differences for every call to an unlinked sibling, and the real residue can sit in a later hunk.

### 2.97 Lock-guarded teardown and ramped counter matched

`0x0202d4f0` (116B) is an object teardown under a global lock. The callback and its argument have
to be read into locals before the lock call, otherwise they are re-read after it and the whole
body grows; the flags word on `p+0x34` is re-read after the inner release instead of being cached
across it.

`0x0202d980` (116B) ramps three fields of `data_0204aec0` and then either restarts or finishes.
Two things were needed. The struct type has to cover the whole object from offset 0, so that one
pool load serves `f24` for the call and `f18`/`f1c`/`f20` after it — spelling the fields as
`*(u32 *)(data_0204aec0 + n)` makes mwcc materialise the base twice and costs 8 bytes. And the
wrap test has to be written as separate `+=` statements followed by `if (data_0204aec0.f20 == 0)`,
which puts the long success block in the fall-through path and the small `func_0202d9f4()` block
at the tail; the natural `if (f20 != 0) { ... return; }` inverts the branch and mirrors the layout.

### 2.98 Register snapshot at 0x02026564 matched first try

`0x02026564` (124B) copies a handful of packed fields out of the `0x027ffc80` register block into a
caller struct, then copies two windows of it with `func_020269d0`. The masked reads are bitfield
reads, not `& 7`: retail's `ldrh` + `lsl #29` + `lsr #29` is a three-bit `u16` field and its
`ldrb` + `lsl #28` + `lsr #28` is a four-bit `u8` field, so declaring the block as one struct with
`u16 f64 : 3;` / `u8 f02 : 4;` at their real offsets produces the pairs directly, where `& 7` would
compile to a single `and`.

### 2.99 Display-mode latch at 0x020257e4 matched

`0x020257e4` (120B) returns a cached value unless it is still `-1`, in which case it picks one of
four flag bits from three probe calls and stores `value | (half-word of 0x027ffffa)`. The first
spelling returned the cached word twice (`if (x != -1) return x; ...; return x;`) and cost the same
120B but in the wrong shape: mwcc if-converted the first return into `popne`/`bxne` and gave the
second its own epilogue, while retail branches to the single tail. Folding the guard into
`if (data_020423dc == (u32)-1) { ...compute... } return data_020423dc;` leaves one return statement
and reproduces retail's shared epilogue exactly.

### 3.00 DMA channel start at 0x02026e2c matched, and a lesson about reading pool words

`0x02026e2c` (120B) primes a channel with `func_0202660c(ch, -1, len)`, then unless the flag
argument is zero waits for the DMA control bit to clear at
`0x040000b0 + (ch * 3 + 2) * 4` and calls the ITCM routine `func_01ff8504(ch, src, len, 0xaf000001)`.

Two gotchas. The index has to be written as `(a0 * 3 + 2) * 4`, not `a0 * 12 + 8`: the latter
makes mwcc fold the base into an `mla` (`ldr r1, [pc]; mov r0, #0xc; mla r1, r6, r0, r1`), while
retail keeps `mov r0, #3; mul r1, r6, r0; add r1, r1, #2; add r1, r0, r1, lsl #2`. And the fourth
argument is `0xaf000001`, not `0x0af00001` — I transcribed the pool print by eye, dropped a digit,
and `reg.py` refused the unit while `var3.py` stayed quiet because the differing word is a pool
constant rather than an instruction. Dump the words with `struct.unpack_from` and compare bytes
(`compile_text` vs `retail`) whenever a unit is size-exact and `reg.py` still misses.

### 3.01 Pool-backed node allocation at 0x0201da30 matched first try

`0x0201da30` (112B) asks `func_02018248(*ctx, align32(len) + 0x20, 0x20)` for a block, bails with 0
when the arena is exhausted, attaches the node to the list behind `ctx+4` through
`func_02017770`/`func_020178e8`, fills the four header words at +8/+0xc/+0x10/+0x14 and returns
`node + 0x20` (the payload). Writing the context as a two-word struct keeps `*ctx` and `&ctx->f4`
to a single base register, and the five-argument signature lets mwcc read the last argument off
the stack once, which is where retail's `ldr r2, [sp, #0x18]` between the stores comes from.

### 3.02 Overlay-index removal at 0x02033ab8, and a loop rotation that needed goto

`0x02033ab8` (124B) asserts `nIndex < count` against the strings at `0x02041fcc` ("isdoverlay.c")
and `0x020420d8`, decrements the count, then shifts the remaining 16-byte records down by one.

The shift loop is where the iteration went. As `while (i < n) { p->elem[i] = p->elem[i + 1]; i++; }`
mwcc emitted a pre-test with an if-converted exit (`cmp; addhs; pophs; bxhs`) and the unit came out
136B; a `for (; i < n; i++)` spelling was worse (140B). Retail instead starts the loop with a single
`b` to a bottom test, which is what the explicit rotation
`goto test; body: p->elem[i] = p->elem[i + 1]; i++; test: if (i < n) goto body;` produces. That
spelling is semantically identical to the `while` (zero iterations when `i >= n`) and matches the
bytes, but it is a codegen workaround rather than evidence about the original text.

### 3.03 Element fetch at 0x0202811c matched

`0x0202811c` (124B) advances one 8-byte element: it bounds-checks `index * 8` against the container's
`+0x24`, builds a two-word descriptor {container, base + index * 8} on the stack, hands it to
`func_02028a38` with an 8-byte output buffer, stores the pair into `+0x2c`/`+0x30`, records the index
at `+0x34` and finishes with `return func_02028af4(p, 7);`.

Two things were needed past the first spelling. The container's padding field has to be
`u8 pad0[0x20]` — writing `u32 pad0[0x20]` puts the bound at `+0x84` instead of `+0x24` (same size,
so only the offsets differ). And the output buffer must be declared *before* the descriptor, which is
what puts `out` at `sp+0` and `in` at `sp+8`; the reverse order swaps the two stack slots.

### 3.04 Job submission at 0x0202cabc matched

`0x0202cabc` (124B) takes the lock, refuses with 1 when `data_0204ad00.busy` is already set,
otherwise latches the flag, releases the lock, fills `+0x14 = 1`, `+0x18 = 0`, `+0x8 = arg0`,
`+0x4 = arg1`, `+0x10 = arg2` and returns 0 or 3 depending on `func_0202cc04()`.

The tail is the interesting part. `return func_0202cc04() == 0 ? 3 : 0;` is the same size (124B) and
only differs in two words: mwcc emits an unconditional `mov r0, #3` followed by `movne r0, #0`, where
retail has `movne r0, #0` then `moveq r0, #3`. Writing the condition the way retail tests it —
`if (func_0202cc04() != 0) return 0; return 3;` — gives the two predicated moves in retail's order.
`var3.py` showed only the `bl` lines, so this one needed the byte diff to find; `reg.py` correctly
refused the first spelling.

### 3.05 Subsystem start at 0x0202cb38 matched

`0x0202cb38` (136B) latches `data_0204acfc`, clears five words of `data_0204ad00`, calls
`func_02027d68`, spins until `func_02027f4c(5, 1)` returns non-zero and then arms channel 5 with
`func_02027f74(5, 0x0202c4dc)`.

The first spelling was already 136B but hoisted `ldr` of the object's address above the early
return, because the source opened with `Ctx *o = &data_0204ad00;`. Referring to `data_0204ad00.f0`
directly and leaving the pointer out defers the materialisation to after the guard, which is what
retail does. A local pointer initialised at the top of a function is therefore not free: it moves
the address load earlier.

### 3.06 DMA fill transfer at 0x02026934 matched first try

`0x02026934` (132B) is the fill sibling of `0x02026e2c`: it returns immediately when the flag word is
zero, waits on the channel's control bit at `0x040000b0 + (ch * 3 + 2) * 4`, takes the global lock,
stores the fill word into `0x040000e0 + ch * 4`, calls the ITCM routine `func_01ff844c(ch, &fillreg,
src, (flags >> 2) | 0x85000000)`, releases the lock and waits on the control bit again.

The same factored index `(ch * 3 + 2) * 4` is what keeps retail's `mov #3; mul` sequence, exactly as
in `0x02026e2c`. Retail's `str r6, [r1, r8, lsl #2]!` falls out of keeping the fill register in a
`u32 *` that is then passed to the call, so the address is materialised once with writeback.

### 3.07 Display latch at 0x02032f90 matched first try

`0x02032f90` (132B) writes `p+2` into the two hardware latches at `0x027fff72` and `0x09fe2ffe` when
`func_02033170(1)` succeeds and `p+4` is non-zero, reads `0x09fe20f0` three times into a volatile
local, and finally always writes `*p` to `0x04000204`. None of the four pool words is a relocation —
the relocation and symbol tables are both silent for this function — so they are plain literal
addresses.

The three identical `ldrh`/`strh` pairs exist because the destination is a `volatile u16` local:
without `volatile`, mwcc deletes the dead stores and the unit shrinks. The `a && b` form of the guard
is what produces retail's two separate `beq` to the shared tail.

### 3.08 Flagged device release at 0x0201ca18 matched first try

`0x0201ca18` (132B) tests a one-bit field of `p+0x24`, and when it is set performs the device
release: `func_020271d4(*(u32 *)(p+0x44), 0, 1 << *(u32 *)(p+0x40), 0)`, two teardown calls on
`p+8` and `p+0x14`, a bit clear on `p+0x24`, and a `func_020274c4`/`func_02027580(1)`/
`func_020274f0` sequence around a saved handle. The final `func_0201c9e0(p)` always runs.

Two spellings matter. The flag read is `lsl #0x1e` + `asrs #0x1f`, i.e. a **signed one-bit bitfield
at bit 1**, so the field comes from a union of `u32 w` and a bitfield struct with `s32 f : 1` after a
`b0 : 1`; an `& 2` test would compile to `tst`/`ands` instead. And the clear is a whole-word
read-modify-write through the union's `w`, which gives retail's `add r1, r4, #0x24; ldr; bic; str`.

### 3.09 Overlay band opened: 0x0204ce20 matched on the first spelling

`0x0204ce20` (68B, ov008) is the first unit matched outside `arm9` since the module work began. It
reads `p = *(u8 **)0x0204f440`, tests the byte at `p+0x34`, and on the non-zero path decrements that
byte through a fresh address variable and returns 0; otherwise it tests bit 0x400 of the word at
`p+0x3c`, writing 8 to the byte and returning 1.

Three shapes mattered. Writing the read-modify-write through `u8 *c = p + 0x34; *c = *c - 1;` is what
produces retail's pre-indexed `ldrbne r1, [r2, #0x34]!` followed by `strbne r1, [r2]`. Hoisting
`v = *(u32 *)(p + 0x3c)` above the test reproduces the speculative load retail performs before the
branch. And `return 0` in both exits is what gives `movne r0, #0` before the `bxne lr`.

### 3.10 Compiler versions do not move the allocation ties

`vprobe.py` on `ov002 0x02068c78` and `ov003 0x020b6c88` prints the same register assignment for
1.2/b56 and 1.2/sp2, in both cases the reverse of retail's (`r3`/`r2` where retail has `ip`/`r1`, and
`r2`/`r1` where retail has `r1`/`r2`). So the small-unit allocation residue seen across this session
is a source-shape question, not a build question -- worth knowing before spending a version probe on
another close unit.

Also confirmed: `u8 >> 1` promotes to signed int and compiles to `asr`; write the byte read into a
`u32` first when retail has `lsr`.

### 3.11 A four-member ov003 family around 0x2051c30

Four neighbours in ov003 share one skeleton and all matched with only their constants substituted:
`0x02051c30` (120B), `0x02051ca8` (132B), `0x02051d5c` (132B), `0x02051de0` (120B). Each tests the
u16 at `p+0x18`, and in both arms bumps the u16 at `p+0x10` under a predicate (decrement while
non-zero, or increment while below 0x10), passes it to `func_02021aa8` against a hardware address
(`0x04001050` or `0x04001050`-adjacent pools differ per member), negated in the first arm only, and
returns 0 or 1 depending on the member.

The decrement/increment has to be written as `(*(u16 *)(p + 0x10))--` / `++`, not through a cached
`u16 *c`: the post-increment on a fresh lvalue is what makes mwcc rematerialise the address inside
the branch (`addne r2, r0, #0x10` before `ldrhne`), which is the whole difference between a 3- and a
4-instruction guarded update. The pool word each member arms with is easy to mis-transcribe by one
nibble -- `0x04000050` against `0x04001050` -- and the digest catches it immediately.

### 3.12 Overlay-to-overlay calls need the module-prefixed symbol

`func_ov003_02051638` (124B) matched in `var3` on the first spelling but `reg.py` failed with
`Undefined : "func_02052100"`. The relocation table names the callee
`to:0x02052100 module:overlay(3)`, so the symbol is `func_ov003_02052100`; the disassembler only
shows the bare address. Writing the prefixed name and rebuilding gave a clean digest immediately.
Calls into arm9 (`module:main`) keep the plain `func_020xxxxx` name.

The unit itself: a u16 gate `(u16)(n + 0xffff) > 1` (a pooled 0xffff add, not a subtraction -- the
`add r0, r1, r0, lsr #16` form only appears when the u16 cast is applied before the add), then a
`u16 i` loop from 0x7c to 0x7e calling `func_ov003_02052100(*(u32 *)(base + i*0x1c + 0x9ff8), 0, 0xc0)`.
## 2026-11-22 - nested-index fusion, ov002 0x0205fe04

Direct `b[j*3 + i + off]` spelling reaches 144 B against retail's 140 B with the
address accumulated; a row-base pointer reaches 176 B. Retail's `add r0, r2, ip`
per inner iteration is therefore not reachable from either spelling tried. See
section 3.13 of the playbook.

## 2026-11-24 - the release date is a prior, not proof: one unit really needs `2.0/base`

`func_0201fcc8` (`0x0201fcc8`, 28 B, nine words of a fixed DP layout, `#pragma
thumb on`) is the only entry left in `config/ATOJ/mwcc_versions.txt`, and it is
`2.0/base`. The argument "the game shipped 2004-12-30, so only `1.2/b56`
(2004-09-16) can have built it" fails for this unit, measured directly: with the
entry commented out, `configure.py` emits
`mwcc_exe = tools\mwccarm\1.2\b56\mwccarm.exe` for it, the object is 32 bytes of
Thumb where retail has 28, and the linked ROM hashes to
`2870334109ed2c46ca855170eb4f6369266d0286` instead of
`9eec3248d3569eba8abcb3244adcbdf01a5ea424`. Restoring the entry reproduces the
digest. So the calendar rules out a whole unit built by a later compiler, but the
digest is the only test that decides, and this one 28-byte function genuinely
needs a 2006 build.

Two practical consequences. Multi-build probing (`vprobe.py`, `wallprobe.py`) is
worth its run: this unit is exactly what it finds, and a probe only counts once
the full digest still matches. And an edit to `mwcc_versions.txt` does nothing
until `tools/configure.py ATOJ` re-runs, because `build.ninja` does not list it as
an input.

## 2026-11-24 (same day) - the source is not the problem: 1.2 pools that constant

Retail `func_0201fcc8` (Thumb, 28 B) decoded with capstone:

  movs r3, #1 / lsls r3, r3, #0xc / str r3, [r0] / str r3, [r0, #4] ... str r2, [r0, #0x20] / bx lr

Everything after the third instruction already matches both compilers. The 4-byte
gap is only how 0x1000 is materialised. Measured over every vendored build, same
source file:

| generation | result |
|---|---|
| 1.2/base, 1.2/b56, 1.2/sp2, 1.2/sp2p3, 1.2/sp3, 1.2/sp4 | 32 B: `ldr r3, [pc, #0x18]`, body, `nop`, `.word 0x1000` |
| 2.0/base, 2.0/sp1, sp1p2, sp1p5, sp1p6, sp1p7, sp2, sp2p2, sp2p3, sp2p4 | 28 B, byte-identical to retail |
| dsi/1.1, 1.1p1, 1.2, 1.2p1, 1.2p2, 1.3, 1.3p1 | 28 B but a different body (the later lineage moved the stores) |

Then 20 source spellings of the same store were compiled with b56: literal,
`1 << 12`, `0x10 << 8`, `0x100 << 4`, `0x10000 >> 4`, `(u32)0x1000L`,
`0x800 * 2`, `0x2000 / 2`, `0x1000 | 0`, `0x1000 & 0xffff`, a shift count held
in a u8 or u32 variable, `u32 v = 1; v <<= 12;`, a store through a volatile
pointer, and `p[0] = 1; p[0] <<= 12;`. All of them pool the constant; the two
compound forms add a load and an extra store. No spelling gets 1.2 to emit
`movs`/`lsls` for a constant store.

Conclusion: this is a compiler-generation materialisation policy, not a wrong
source. 1.2 loads any constant that does not fit `movs #imm8` from the pool even
when one `movs` plus one `lsls` would do; 2.0 gained the shift form. The unit
keeps its `2.0/base` override, which AGENTS.md permits because the digest still
matches. If a future reader wants to revisit it, the test is cheap: recompile
this file with each build and compare the first four bytes.

## 2026-11-24 (later) - solved: not a compiler at all, it is NitroSDK asm

The Nitro SDK archive (twlsdk.randommeaninglesscharacters.com) settles this. The
9-word Thumb block is NitroSDK `MTX_RotX33_`, and in the SDK itself it is a
hand-written asm block in `build/libraries/fx/src/fx_mtx33.c`, wrapped in
`#include <nitro/code16.h>`:

    asm void MTX_RotX33_(register MtxFx33 * pDst, register fx32 sinVal, register fx32 cosVal)
    {
        mov r3, #1
        lsl r3, r3, #12
        str r3, [r0, #0]
        mov r3, #0
        str r3, [r0, #4]
        ... neg r1, r1 / str r1, [r0, #28] / str r2, [r0, #32] / bx lr
    }

Assembled by `mwasmarm` and shipped prebuilt, which is why no mwccarm build
produces this shape from C: the `movs`/`lsls` materialisation is the SDK's own
instruction sequence, and the constant never reaches a compiler. Written as the
same `asm void` block, `1.2/b56` assembles it byte for byte (28 B, verified), so
`src/arm9/func_0201fcc8.c` no longer has any override and
`config/ATOJ/mwcc_versions.txt` is empty again. The lesson for the playbook: a
constant-materialisation difference that no build and no source spelling explains
is a cue to check whether the unit is a prebuilt library routine, not a cue to
change compilers.

Two neighbours fell out of the same file. `MTX_RotY33_` at 0x0201fce4 (28 B) and
`MTX_RotZ33_` at 0x0201fd00 (24 B) had been logged as "twins" blocked on
register roles under the 2.0 theory; written from the SDK source as asm blocks
both assemble exactly under b56 and are now installed. That is +2 functions,
+52 B, and the first evidence that this region of arm9 is linked NitroSDK
library code rather than game code.

Proof from the prebuilt libraries, not just the source. In NitroSDK the fx
library ships as `lib/ARM9-TEG/Rom/libfx.thumb.a`, member `fx_mtx33.o`. The
retail 28 bytes at arm9 0x0201fcc8 appear verbatim at 0x35a0 of that member, and
the `MTX_RotY33_` body at 0x35d0, so retail linked prebuilt NitroSDK code rather
than any Konami-built object. The identical bytes are also in NitroSDK 1.0
(2004-04-16) `libfx.thumb.a`, so this function does not discriminate between SDK
releases; pinning the exact release needs a routine that changed between them,
which is a separate sweep.

## 2026-11-24 - pragma optimize_for_size is not a lever for the allocation ties

Tested on arm9 0x02024a54 (144 B, 22-channel handler-table initialiser, the
lr-as-table-base tie). Four source shapes: plain if/else (140 B), nested if
(140 B), fused condition (148 B) and two separate `if (e)` tests (144 B - the
size retail has, 19 slot diffs). Adding `#pragma optimize_for_size on` or `off`
to the best shape changes nothing at all: same 144 bytes, same 19 slots,
byte-identical output. So the pragma is not the missing lever for this class;
the residue is still register role (retail's table base in `lr`, mwcc's in
`r5`/`r4`). Parked with the body in build/cand/f02024a54.c.

## 2026-11-24 - the one-register residue is not a compiler-version signal

`arm9 0x0202c430` (spin on `*(volatile u32 *)0x0204ad00 == 1`) differs from
retail in exactly one register: retail loads the address into `ip`, every
spelling mwcc gets produces `r1`, and the pool value, loop body and comparison
are identical. To rule out a build difference the same source was compiled by
**all 25 vendored compilers** (`1.2/{base,sp2,sp2p3,sp3,sp4,b56}`,
`2.0/{base,sp1,sp1p2,sp1p5,sp1p6,sp1p7,sp2,sp2p2,sp2p3,sp2p4}`,
`dsi/{1.1,1.1p1,1.2,1.2p1,1.2p2,1.3,1.3p1,1.6sp1,1.6sp2}`): every one emits the
same 24 bytes, `ldr r1`. So this class of residue is not "retail used a later
CodeWarrior" - it is something about how the original source reached the
allocator, and a compiler sweep will not close it.

Two related observations while sweeping the queue:

* `sdk.py` puts 0x0202c430 at 21/24 bytes of `libspi.a(mic.o)` / `librtc.a(external.o)`,
  so it may be a library routine rather than game code; `func_02026ac8` beside it
  turned out to be a second copy of `MI_Copy64B` (already matched elsewhere), which
  is why `tools/match/true_fresh.txt` still lists it.
* `true_fresh.txt` is stale in general: three of its arm9 entries were already
  matched. A queue rebuilt from `symbols.txt` minus `src/arm9/<name>.c` finds **393**
  arm9 functions without a source file (383 above 6 bytes), and only the ten
  2-mod-4 thumb stubs in that list are blocked by the alignment issue.
* Several of the small remaining units (`0x0202612c`, `0x0203b4b0`) start with a
  bare `stmdb sp!, {lr}` and never align the stack to 8, where our C compiles
  `push {r4, lr}` plus `sub sp, sp, #4`. That is the AAPCS frame mwcc emits for
  ordinary C, so these are more likely asm blocks than C - and `bl` to a C symbol
  inside an `asm void` body does not assemble, so transcribing them needs the
  calls expressed differently.

## 2026-11-24 - the "frame-less asm" seam is three thunks, not a class

Sweeping the rebuilt arm9 queue for units that open `stmdb sp!, {lr}` and then
*do not* realign with `sub sp, sp, #4` found **exactly zero** remaining
candidates: every other call-making unit realigns normally and is ordinary C, so
there is no asm-block family to mine. The three that match that shape were
`0x0202612c`, `0x0203b4b0` and `0x0203b4e4`, all now matched by writing the
`stmdb`/`ldmia` bodies directly (112 B total). Two details that took a try each:
`ldm` inside an `asm void` body is rejected and must be written `ldmia`, while
`bl <extern C symbol>` assembles fine.

The neighbouring `0x02033bc0` / `0x02033c0c` pair is *not* a pair of tail-call
stubs as its disassembly suggests. Both are `mov r0, #N; b func_02033bd0`-shaped
in the disassembler, but the second branch word is `0xffffffea`, which is
`cond = 0xf` and cannot be a real branch; retail's implied target lands inside
the preceding function. These two look like mis-delimited unit boundaries rather
than code to reconstruct from the listing, so they are left alone.

## 2026-11-24 - asm void syntax details, and the itcm module

`func_02025698` (68 B) is the ARM interrupt-return trampoline - `msr
cpsr_c`/`spsr_fsxc`, `ldm r0, {r0-r14}^`, `subs pc, lr, #4` - and it assembles
straight from the retail listing once two syntax points are known:

* `mrs r1, apsr` is **rejected** by mwccasm in this toolchain; write `mrs r1, cpsr`
  and the encoding is identical (`0xe10f1000`).
* `ldmia r0, {r0-r14}^` is accepted as written, and `subs pc, lr, #4` too.

So "hand-written asm we cannot reproduce" is not a fair description of these:
the listing transcribes one-for-one, and the digest decides.

The `itcm` module has no matched units at all (11 symbols, 1,520 B), and its
delinks boundaries are **not** function boundaries: `0x01ff8000` starts with
`sub ip, r0, #0x36`, uses `r4`-`r8` before setting them, and contains several
`pop {r4, r5, r6, lr}` tails, so the whole 0x01ff8000-0x01ff85f0 span is one
switch-dispatch routine that dsd has cut into eleven fake units. Matching it
requires re-delimiting `config/ATOJ/arm9/itcm/{symbols,delinks}.txt` to the real
entry point first; the eleven current "functions" cannot be written as sources.

The `0x02033e40` / `0x02033e98` / `0x02033ef4` / `0x02033f90` cluster (88/92/76/72 B)
all funnel into `func_02033fd8` while building a 5-word local block around a
`sp-4` scratch pair, then returning one or two words of it. That is mwcc's shape
for a varargs pass-through, and the argument mapping is not recoverable from the
listing alone (in `0x02033f90` the fourth argument register ends up holding the
first argument), so the four are parked rather than guessed at.

## 2026-11-24 - what the itcm section actually is

The whole `itcm` .text (0x01ff8000-0x01ff85f0, 1,520 B) is **byte-for-byte the
same instruction stream as arm9 0x02036a60-0x02037050**: all 380 words compare
equal. Its eleven "functions" are therefore not functions at all - they are
slices of four arm9 units that dsd has cut apart:

    func_02036934  0x02036934  740 B   FRESH
    func_02036c18  0x02036c18  208 B   FRESH
    func_02036ce8  0x02036ce8  600 B   FRESH
    func_02036f40  0x02036f40  992 B   FRESH

The copy does not start on a function boundary: 0x02036a60 is 0x12c into
func_02036934, which is why the first itcm unit begins with `sub ip, r0, #0x36`
and touches r4-r8 before setting them. There is no prologue anywhere in the
region and no jump table, and the branch encodings are arm9-absolute - `bl
#0x1ff84e0` only decodes to an in-ITCM target because those same bytes live at
0x0203xxxx in arm9 - so the blob is a genuine second copy of the same code,
linked for the ITCM address rather than re-relocated.

Consequences: matching those four arm9 functions (2,540 B) is the precondition
for the itcm module, and the itcm side cannot be written as ordinary C at all,
because the bytes it must reproduce branch to arm9 addresses. Whatever solves it
will be a build-system alias for a duplicated section, not a source file.

## 2026-11-24 - Ghidra settles the shape of the digit formatter

Headless Ghidra (`M:\ATOJ-tools\proj`, project `ATOJ9`, program `arm9.bin`,
`build/ghidra/dump_itcm.bat`) decompiled the four itcm-origin units. For
`func_02036c18` its structure is exactly the one that reproduces the 208-byte
retail size:

    n    = *(s16 *)(a + 2);
    f(s);                                  /* func_02036ce8 */
    if (*(u8 *)  (s + 5) > 9) return;
    if (n > 0x20) n = 0x20;
    g(s, n);                               /* func_02037660 */
    cnt  = *(u8 *)(s + 4);
    if (cnt < n) {
        do {
            *(u8 *)(s + 4) += 1;
            *(u8 *)(s + cnt + 5) = 0;
            cnt = *(u8 *)(s + 4);
        } while (cnt < n);
    }
    i = 0;
    *(s16 *)(s + 2) -= (cnt - 1);
    if (*(u8 *)(s + 4) != 0) {
        do { *((u8 *)s + i + 5) += 0x30; i++; } while (i < *(u8 *)(s + 4));
    }

Caching the pre-increment count in a local and re-reading the field afterwards
is what makes the loop body match: every variant that indexed with
`f5[*p - 1]` or incremented before the store came out 216 B.

The only residue left is the callee-saved assignment: retail spills the pointer
parameter to `r5` and keeps `n` in `r4`, mwcc does the reverse (`mov r4, r3` /
`ldrsh r5, [r0, #2]`). Declaration order, `s16` vs `s32`, `register`, an
`S *` local alias for the parameter and a `void *` parameter were all tried and
every one emits `mov r4, r3`. Classic two-value allocation tie; parked.

## 2026-11-24 - the ov002 small units were already walls

The 24 unmatched ov002 functions of 120 bytes or less looked like fresh ground
because the module has no parked entries I had touched; three of them turned out
to be parked already, and working them from scratch reproduced the same walls:

* `0x02081078` (60 B) - `[*(u8 **)0x02102070 + i + 0x3d]` with an early return.
  Twelve spellings give a function that is byte-identical except for one
  register: retail keeps `i` in `r0` and uses `r2` as the scratch, mwcc uses
  `r1` and clobbers `r0`. Included `u32 idx = i + 0x3d`, struct-field forms,
  `volatile`, a pointer walk (`p += i; p[0x3d]`), and a two-argument prototype;
  the `r2` allocation only appears together with the wrong instruction order.
* `0x0206308c` (56 B) - two calls, then set bit 0 at `0x027c3ff8`. Retail keeps
  **two** literal-pool words (`0x027c0000` and `0x3ff8`) and does
  `ldr r0, [r1, r0]!`; mwcc always folds the pair into `add r1, r0, #0x027c0000`
  and one literal, giving 52 B. Tried `u32 *` and `u8 *` bases, a struct with a
  0x3ff8 pad, `volatile`, separate `base`/`off` locals and an explicit
  `p += 0xffe` - every one folds.
* `0x02068c78` (64 B) - stride-0x158 table walk; best 72 B.

A quick sweep of all 25 vendored compilers on the first one settles that this is
not a build difference: `1.2/sp3` and `1.2/sp4` change the register and the code
layout (they emit `ldr r1, [pc, #0x20]`), and everything from 2.0 onward emits
`ldr r0` with a different displacement, but none produces retail's `r2`.

## 2026-11-24 - arm9 interrupt state-save family, and what `func_02036934` is

Five arm9 units of the CPU-state block at `0x02048f84`-`0x02048f90` matched, all
first-try once the shape was right:

* `func_02026140` (144 B), `func_020261d0` (108 B), `func_0202612c` (20 B):
  register-save / mode-switch routines. They read `r12` as an implicit input,
  save banked `sp`/`lr`/`spsr` and toggle `cpsr_fsxc`, so they are `asm void`
  bodies (the same route as `func_02025698`), not C.
* `func_0202623c` (144 B), `func_02026738` (148 B), `func_020274f0` (144 B): C.
  `func_02026738`/`func_0202623c` confirm the `||` rule (§4.1): the shared tail
  only appears when the guard is written `a == 0 || b`, never as nested ifs.

Assembler facts found while writing those:

* `ldmeq sp!, {lr}` and `ldmiaeq sp!, {lr}` are both rejected. The accepted
  conditional form puts the condition before the addressing mode:
  `ldmeqia sp!, {lr}` -> `08bd4000`. Same for `ldmeqia sp!, {r14}`.
* `ldr rX, =0x9f` pools a plain integer fine; no symbol needed.
* Taking the address through a local pointer variable
  (`u8 *base = (u8 *)0x027c3000; *(u32 *)(base + 0xfdc) = v;`) is what keeps a
  non-relocatable base out of the literal fold. Writing `*(u32 *)0x027c3fdc`
  directly, or indexing `((u32 *)0x027c3000)[0x3f7]`, both fold into one pool
  word and lose the `#0xfdc` offset - that single instruction was the only
  difference in `func_0202623c` between 3 diffs and a match.

`func_02036934` (740 B, itcm origin) is decoded: it is an `ldexp`-style
`x * 2^n` for a soft-float double in `r0:r1` (arg `n` in `r2`). Constants, in
order of first use: `0x7ff00000`/`0x000fffff` exponent and mantissa masks,
`0x4350000000000000` = 2^54, `-50000`, `1e-300` (`0x01a56e1fc2f8f359`),
`0x7ff`/`0x7fe`, `1e300` (`0x7e37e43c8800759c`), `0x800fffff`, `50000`,
`2^-54` (`0x3c90000000000000`). It classifies the operand (zero/subnormal,
normal, inf/NaN), pre-scales subnormals by 2^54, adds `n` to the exponent and
reassembles the word, overflows through `1e300 * copysign(1e300, x)` and
underflows through two `1e-300` multiplies so the sign survives.

The best candidate (`DU` aggregate parameter, local copy, body on a union) is
exactly 740 B with the same instruction multiset, 123-128 differing slots:
retail keeps `lo`/`hi` in `r5`/`r4` (loaded from the aggregate's home slot) and
works on that home at `[sp+0x18]`, while mwcc puts the working value in the
local at `[sp]`, uses `r1` for `lo`, and lays the `exp == 0` body inline where
retail branches to it out of line. Fifteen spellings (aggregate vs `double`
parameter, locals before/after, dead copy, `!=`/`==` first test, `? :` vs if)
all keep that shape; this is the parked one-register/layout class, not a
missing insight.

Two neighbours stay parked for the same reason: `func_020266c0` (80 B, 16
slots, retail scratch `r1`/`r2` vs mwcc `r0`/`r1`) and `func_020267cc` (192 B,
best 204 B; the pre-indexed `str r4, [r1, r6, lsl #2]!` needs the store address
to stay live as the call's `r1`).

### 2.105 2026-11-24 - four spellings that decide four recurring shapes

Five spellings were measured on 1.2/b56 while matching `func_ov006_0204ddc0`,
`func_ov006_0204e3b8`, `func_ov008_0204d9dc` and `func_ov006_0204de34`:

* **Compound assignment keeps `mul`.** `p->field *= -1;` on an `s8` member emits
  `add r2, p, #0xe / ldrsb r1, [r2] / mvn r0, #0 / mul r0, r1, r0 / strb r0, [r2]`,
  where `p->field = p->field * -1;`, `p->field = -p->field;` and
  `p->field = p->field * (s8)0xff;` all collapse to a single `rsb` (36 B each vs
  44 B for the compound form). Retail's negation in `func_ov008_0204d9dc` is the
  compound form.
* **An 8-byte struct copy is a byte loop.** `B8 tmp = data_X;` with
  `typedef struct { u8 b[8]; } B8;` expands to four iterations of
  `ldrb r1, [ip], #1 / ldrb r0, [ip], #1 / subs r2, r2, #1 / strb r1, [r3], #1 /
  strb r0, [r3], #1`, which is the opening loop of `func_ov006_0204de34`. A
  declared `memcpy(buf, src, 8)` is not intrinsified here: it stays a call
  (140 B against retail's 156 B).
* **A source-visible memory object spills.** A `volatile u32` local holding the
  constant `0xc0` gives `str r1, [sp]` ... `ldr r0, [sp]` around the call, the
  shape retail has in `func_ov006_0204e3b8`; a plain `u32` local is folded to
  `mov r0, #0xc0` and loses the match. The same spill explains the `strh/ldrh`
  round trip of the zero argument in ov005 `func_ov005_0204dc7c`.
* **Re-read the pointer global after the call.** `func(g + 0x11d8)` where `g` is
  an `extern u8 *` produces retail's three separate `ldr rX, [pc]` loads;
  caching `g` in a local keeps it in `r5` across the call and adds `push {r5}`.

Residue of the same batch, both parked: `func_ov008_0204d9dc` is 124/124 B with
an identical instruction stream but colours the struct pointer `ip` where retail
uses `r4`, and `func_ov006_0204de34` pays a 4-byte frame for colouring the copy
source `r4` where retail uses `ip`. The local-vs-assigned declarations, `u32` vs
`s32` temporaries and statement reordering tried on both do not move the colour.

* **A switch does not become a range test.** `switch (x) { case 0x59: case 0x5a:
  k = 1; break; }` keeps retail's `cmp r0, #0x59 / beq L / cmp r0, #0x5a /
  bne M / L: mov r2, #1` with the shared body, where `x == 0x59 || x == 0x5a`
  is strength-reduced to `sub r0, r0, #0x59 / cmp r0, #1 / movls r2, #1` (116 B
  both sizes, different bytes) and the `if/else if` form if-converts each arm to
  its own `moveq`. This decided `func_ov005_0204dc7c`.

* **A pointer value is not an addressing mode.** `*(u16 *)(p + 0x36e)` is folded
  into `add rX, p, #0x300` plus the `[rX, #0x6e]` immediate, because mwcc splits
  the offset into a form the addressing mode can carry. Assigning the address to
  a pointer first - `c = (u16 *)(p + 0x36e); *c = *c + 1;` - makes the sum a
  value, so 1.2/b56 emits the pool constant `0x36e` and `add r2, p, r1`, which is
  what retail has. Both spellings are 8 bytes apart and compile to different
  bytes; this decided `func_ov009_020517f0`. The same distinction explains the
  `ldr r1, =base / ldr r0, =0x3ff8 / ldr r0, [r1, r0]!` shape in ov008
  `func_ov008_0204e51c`, which still resists: with a symbol or literal base mwcc
  folds base+offset into one pool word.

* **`volatile` on the pointee keeps a global's field loads.** ov008
  `func_ov008_0204cf80` re-reads `p[0x30]` after the call instead of keeping it
  in a register; declaring `volatile u8 *p = data_ov008_0204f440;` is what
  produces those loads (the plain `u8 *` cached the byte and lost 4 bytes).
  Found by `tools/match/respell.py`, which walks declaration order, init versus
  assignment, compound assignment, `volatile` and dead pads in one run.
* **A pointer-typed local makes an address CSE-able; an integer base does not.**
  In ov009 `func_ov009_0204deec` the same `t + 0x1e4c0` is an argument twice.
  With `u8 *t` mwcc hoists the sum into a callee-saved register and reuses it
  (148 B, wrong); with `u32 t = (u32)data_ov009_02054f80;` and `t + 0x1e4c0` it
  re-materialises the pool constant and re-adds per call, which is retail's
  156 B.

* **The candidate queue lied, again.** `tools/match/true_fresh.txt` still listed
  ov004 `func_ov004_0204cbdc`, which has had a source since long before this
  session; six candidate spellings were written for an already-matched unit
  before `grep src/` caught it. `tools/match/fresh.py` now derives the queue from
  the tree each run - every `func_` symbol in `symbols.txt` that no
  source-backed `.text` range in `delinks.txt` covers - and `--write`
  regenerated the snapshot (2,263 entries, 190 of them stale, down to 2,073).
  Both walls retried this session, ov009 `0x0204f93c` and ov004 `0x0204d980`,
  were already parked in `walls.txt`; read the wall list before writing C for a
  small unit, not after.

* **A clamp that looks like a billion is `999999999`.** arm9 `func_0200fcac`
  keeps a signed score at `+0x8`, adds its argument to that and to `+0xc`,
  clamps to +/-1e9 and returns it. Every instruction matched with
  `1000000000`; only the two trailing pool words differed (`0x3b9aca00` against
  retail's `0x3b9ac9ff`, and `0xc4653600` against `0xc4653601`), because both
  constants are materialised as literals. An instruction diff cannot see a wrong
  *value*, so a residue living in the pool words is a constant to re-derive
  rather than a colouring tie: dump both words and compare the numbers. The
  clamp is also what decides signedness - `s32` on the parameter and on the
  field reads turns `beq`/`strhi`/`strlo` into retail's `ble`/`strgt`/`strlt`.

* **A size recorded in a journal or a wall line goes stale; the delinked range
  does not.** `grind.py --retry-slots` first scored the near-misses against the
  size from the record, so ov001 `0x0204fc1c` was tested as 500 B while the
  delinked unit is 4 B, and its "1 slot" was the residue of the wrong length.
  Retry mode now prefers `fresh.unmatched`'s live size. Those 4 B entries are
  symbol-boundary fragments inside a larger routine, not functions - a candidate
  for one cannot be `ldr r0, [pc, #0x1e4]`.

* **A stack argument is only a stack argument once the pushes are counted.**
  With `push {r0-r3}` + `push {r4-r7,lr}` + `sub sp,#4`, the homed a0-a3 sit at
  entry_sp-16..entry_sp-4 and the 5th argument sits at entry_sp+0, so
  `ldr r2, [sp, #0x28]` there is a4 and `[sp, #0x34]` is a7 - both are frame
  offsets plus the 40 bytes the prologue pushed, not locals. Reading them
  against the bare sp offset turns them into slots that never match, and a
  re-write back (`str r0, [sp, #0x28]`) is an assignment to the parameter.
  `tools/match/auto.py` now carries sp as a depth with its own kind (`('sp', n)`,
  never a plain constant, or `add r2, sp, #4` folds to the integer 4) and
  identifies a frame cell by how far below entry_sp it sits, so it survives the
  prologue pushes and the epilogue pops. The three stack bail classes
  (`load from sp` 268, `store above frame` 134, `implausible stack argument` 29)
  fall to zero and the generator reaches 46 more units - none of them matched in
  a 400-variant sweep, so this unlocked the class without winning it: most of
  those units are argument-staging copies that still need `ldm`/`stm` and the
  live-result conditional runs before their C is the original's shape.
