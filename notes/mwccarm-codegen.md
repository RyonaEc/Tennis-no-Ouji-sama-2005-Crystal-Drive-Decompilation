# mwccarm codegen levers (ATOJ)

Distilled, reusable index of source-shape levers verified against this cartridge.
Function names appear only as evidence pointers. Nothing here *predicts* the
allocator: every lever is an axis to move, and every candidate must be compiled
and byte-diffed before it counts.

Toolchain for this repo: CodeWarrior `mwccarm` 2.0/sp1p5, `-O4,p`, ARM mode,
`struct` alignment 4. The ROM is 2004-12-30 vintage; CodeWarrior builds of that
era behave like the 2004 `b56` line in some places (see §6).

## 0. How to work

* A function counts only when the whole rebuilt ROM still hashes to
  `8d555a80db47278475ffdfdf808fe80dbd7c48cf`. `python -m ninja sha1` after every
  accepted unit; nothing else is proof.
* Compile one unit, diff it, fix the **first** structural divergence, then look
  at colours. Do not chase the diff count.
* Read register *roles*, not register numbers: which web owns which register and
  in which order it is written. A whole-function rename is one problem, not N.
* Two spellings that "should" be identical frequently are not (see §2, §3). The
  cheapest experiment is usually reordering statements, not rewriting logic.
* Keep a wall list (§6). A wall is a function whose *shape* is source-reachable
  but whose instruction selection is not, with the evidence that says so.

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
| Your function is 8–16 bytes shorter, an argument copy is missing | retail keeps a fourth value live; see §3.4 | 3.4 |
| `strh`/`strb` immediate split into `#0x3000` + small offset | the source constant is a value, not a literal; use a `u32 off` local | 2.4 |

## 2. Addressing and memory shapes

**2.1 Materialised vs folded pointers.** `*(T *)(base + off)` folds `off` into
the load/store when it fits. Retail materialises (`add rX, base, #off` then
`[rX]`) in a subset of functions — mostly read-modify-write of one flag word
where the *same* computed address feeds both the load and the store. A local
`T *q = (T *)(base + off);` used for both halves is the strongest C-side lever;
when that still folds, the function belongs in §6. Evidence: the
`func_ov002_0204d09c` five-function set (folds here), `func_ov002_020633e0`
set and `func_ov002_0207b5f4` (materialise here).

**2.2 Count the base loads.** Retail frequently re-reads
`data_XXX_02102070` mid-function. Caching it in a local removes the second
`ldr [pc]`/`ldr [rX]` pair and shortens the function by 8–16 bytes. Writing the
global expression twice restores it. Evidence: `func_ov002_0206e024` matched
only after the second spelling; `func_ov002_0209e44c` needs the same.

**2.3 Store-then-reload.** Retail sometimes stores a field and immediately
re-loads it (`strb r0,[r4,#2]; ldrb r0,[r4,#2]`). That is the compiler, not
`volatile`: write the dereference twice rather than holding a local.

**2.4 Literal constants vs values.** A constant that is not encodable as one
`mov` comes from the literal pool. If retail splits an offset into
`#0x3000` + small immediate instead of using a pool word, the source had the
offset as a *value* (a `u32 off` local), not a literal in the expression.

**2.5 Byte copies.** `ldrb/ldrb/strb/strb` with `[rX], #1` post-index is not
reachable from C here; every spelling tried folds to offset addressing or
`add`+copy. See §6. A 2-word adjacent copy *is* reachable as a struct
assignment (`*(P2 *)dst = *(P2 *)src;`), which batches both loads then both
stores.

## 3. Register allocation

**3.1 Order decides colour.** Callee-saved locals that live across calls are
numbered from the last declaration backwards; statement order of *first demand*
decides the rest. Permute declarations before touching anything else. When the
whole trio is one register low, the missing pressure is usually a value retail
kept live across the region (§3.4), not a spelling error.

**3.2 Narrow callee parameters.** Declaring an extern as `void f(u8)` /
`void f(s32)` moves the conversion to the call site (`and r0, r4, #0xff`,
`mvn r2, #0`). Declaring it wide makes CodeWarrior hoist one narrowed copy of
the caller's parameter. Evidence: `func_ov002_020e4e2c`, `func_ov002_020788a8`.

**3.3 Signedness picks the condition code.** `s32` operands produce
`movge`/`movlt`; `u32` produces `movhs`/`movlo`. Evidence:
`func_ov002_02080eac` differed by exactly those two instructions until both the
load and the parameter became signed.

**3.4 Register-shift recipes that failed here.** Adding `(void)param;`,
declaring an extra unused temp, and re-ordering *only* declarations did not
move a one-register-low trio in eight attempts on the `func_ov002_0204d09c`
and `func_ov002_020a542c` sets. Do not spend more than a handful of attempts
on this class; record it and move on.

## 4. Control flow and shifts

**4.1 Branch vs predication.** `return !(a) || (b);` emits a real branch
chain; the equivalent early-return `if` ladder emits `movcc` predication.
Evidence: `func_ov002_0207a93c` matched only in the `||` form.

**4.2 Loop shape.** A `for (i = 0; i < n; i++) { *p = v; p += k; }` keeps the
counter increment before the pointer bump, matching retail. Post-index stores
appear when the step is larger than the store width and written as its own
statement (`*(u32 *)p = v; p += 8;` → `str r,[r0],#8`). Evidence:
`func_ov002_02086f7c`.

**4.3 Switches.** Case *body* layout follows source label order, so write the
labels in the order retail's blocks appear. A dense switch whose low cases are
absent can lower to a compare chain; a `case` ladder returning a constant
lowers to range check + equality chain. Evidence: `func_ov002_0205078c`.

**4.4 Signed division by a power of two** on a 32-bit value lowers to
`add rX, rX, rX, lsr #31; asr rX, #1`. Writing `/ 2` on an `s32` produces it;
`>> 1` on the same value does not.

## 5. Data symbols

* `extern E data_X[]` plus `&data_X[i]` gives the pooled address form; a
  `extern u32 data_X;` gives a direct load.
* A read-only table indexed by a byte offset (`data_X[k * 2]`) with an `s16[]`
  declaration produces `ldrsh`. Evidence: `func_ov002_0204f770`.
* Tables of structs use `mla rD, index, stride, base` when stride is not a
  power of two (`0x158`, `0x1c`, `0x38`, `0x44`), and a scaled index otherwise.

## 6. Known walls

Recorded so the same ground is not re-covered. Each was swept across source
idioms; none is a skill gap.

* **Materialised flag RMW.** `add rA, rB, #off; ldr rV, [rA]; orr/bic; str
  [rV]` — C always folds. Swept: deref, pointer local, index, split-RMW,
  `volatile`, u64-mask launder (two forms). Blocked: `func_ov002_020633e0`,
  `02063820`, `02063c5c`, `0207a93c` (tail), `0209e44c`/`020ace70` (tail),
  `0204c824` (tail).
* **Folded halfword address in `return (*p += 1);`.** `func_ov002_02072324`
  wants `add r1, r0, #0xa; ldrh r0, [r1]; add r0, r0, #1; strh r0, [r1]`,
  while C folds the base into the load/store (`ldrh r1, [r0, #0xa]`, two
  accesses, then a reload for the return). Same class as the wall below.
  Swept: pointer local declared inline and separately, separate assignment.
* **Predicated if/else with a re-materialised else address.**
  `func_ov002_0204d488` tests `*(u16 *)(b + 0x30048) >= 0x1f`, stores 0 when
  true and otherwise re-loads and increments. Retail builds the test address
  as `add r0, r2, #0x30000; ldrh r1, [r0, #0x48]`, but the else branch loads
  `0x30048` from the pool, adds it to `b`, and does a fresh `ldrh`/`add`/
  `strh` instead of reusing the tested value. C CSEs both spellings into one
  address and keeps the tested value (`addlo r1, r1, #1`). Swept: `> 0x1f`
  vs `>= 0x1f`, both spellings in both branches, split load/store, `volatile`
  pointer. Not solved.
* **Two-load/two-store byte copy with `[rX], #1`.** Blocked: `func_02006be0`,
  `02006c10`, `02006cd0`, `02008a60`, `0200e99c`, `020080d0`, `0200e8f4`,
  `02010248`.
* **Pre-indexed writeback** `ldr rV, [rB, rI]!`. Blocked:
  `func_ov002_0207808c`, `020781b4`, `02086020`, `02085ff8`, `02062b20`,
  `0206308c`, `func_02033cb0`.
* **One-register-low temp trio with an inverted branch.** Still blocked:
  `func_ov002_0204d09c` only. (The rest of that set — `0204d0e0`, `0204d12c`,
  `0204d1e8`, `0204d22c`, `0204d270`, `0204d2b4`, `0204d2f8` — is solved; the
  missing lever was the live callee result in §10. The `0x5c`/`0x60`
  state-machine sets are solved too — see §9.)
* **Signed word-fill loop.** `func_020048e8` wants `cmp r1, ip; stmlt r1!,
  {r0}; blt` (signed test, store-multiple writeback). C gives `cmp` + `bxhs`
  + `str r0, [r1], #4`. Swept: pointer local, `(s32)` casts on both sides,
  struct-valued fill, byte stepping. The `lt` test and the `stm` writeback
  are separate missing levers.
* **MMIO address from the literal pool.** `func_02024ae4` (`0x027c0000`)
  wants `ldr r0, [pc, #8]`; C builds the constant in a register instead
  (16 B vs 20 B). Same shape as the pool-address wall in §2 but with an
  absolute address, not a symbol.
* **`mrc` result register.** `func_0202607c`: `asm { mrc p15, 0, v, ... }`
  allocates the destination as a temp (`r1`) plus a copy; retail reads
  straight into `r0`. Also the mask `0xfffff000` must come from the pool,
  while C canonicalises `& 0xfffff000` to `mov`/`rsb`/`and`.
* **Adjacent bare calls get a frame.** `func_0202612c` (two `void` calls,
  nothing else) wants `stmdb sp!, {lr}` + `bl` + `bl` + `ldm`; C inserts
  `sub sp, sp, #4` / `add sp, sp, #4`. Reserved-stack alignment, not source.
* **`b` local in the family clone colours r4/r5 backwards.**
  `func_ov002_020c08e8` is §9's template plus one extra store
  (`*(u8 *)(b + a0 + 0x14a) = 0;`) and needs `b` in `r4`, `p` in `r5`.
  All declaration orders tried put `b` in `r5`. Membership test for §9 is
  therefore: no `b` local, or an exact colour match.

### Levers found while cracking the §9 clones

* **Compound assignment fixes evaluation order.** `s32 d = A - B;` makes CW
  load `B` first and `A` second (right-to-left). Writing
  `s32 d = A; d -= B;` loads `A` first, which is what retail does — this
  matched `func_ov002_0206394c` on the first try after the two-load form failed.
* **Offset before scaled index.** In a loop body, `*(u16 *)(t + i * 8 + 6)`
  and `*(u16 *)(t + 6 + i * 8)` are not the same unit: the second form is what
  matches `func_ov002_02063168`, and it also moves the hoisted constants up one
  register pair (`r2`/`r3` instead of `r1`/`r2`).
* **Family clones with an extra store keep `B` un-cached.** `func_ov002_020daa24`
  matches only when the template's `B` macro is used twice instead of a `u8 *b`
  local; with the local the pointer lands in `r2` and the slot product in `r1`,
  the reverse of retail. (Contrast `020c08e8`, still blocked, which needs the
  local but the opposite colouring.)
* **`r = 1; *(u8 *)(p + 0xd1) = r;`** — when state and flag are the same
  constant, storing the *variable* after assigning it matches (`mov r3,#1;
  strb r3,[r4,#0xd1]`), unlike the plain §9 template which stores the state
  first.

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
reloc-aware instruction diff against retail without touching the tree; it is
the fastest way to read a residue. `build/auto.py` exposes `set_module`,
`FUNCS` (addr → name, size), `SYM`, `disasm(addr, size)`.

## 9. Solved: the per-slot state-machine family (21 functions)

Twenty-one `0x5c`/`0x60` functions share one skeleton: `mul` by `0x158`, a
`*(u32 *)(p + 0xd8) & mask` gate, `strh 0` to `p + 0x66` (and sometimes
`p + 0x6a`), a call taking `*(u8 *)(b + n + 0x2e6)`, `*(u8 *)(p + 1)` and a
literal, then a state byte at `p + 0xd1`, then `return`.

What moves the residue is the **exact statement order at the tail** and keeping
the `0x2e6` byte as a *named local declared after* the `s32 r = 0;`:

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
branch into an early return and shift the trio one register down. Matched
set: `020a542c`, `020e28b8`, `020eac6c`, `020f1a94`, `02099f40`, `0209b8ec`,
`020a16d8`, `020a17bc`, `020a385c`, `020a67c0`, `020ac768`, `020b2a04`,
`020b2a64`, `020b5c44`, `020b9b78`, `020b9bd8`, `020bcbc4`, `020c9be8`,
`020cb768`, `020d1314`, `020d2da0`.

## 10. Solved: the `0x274` per-slot flag setters (8 functions)

Eight ov002 functions index a global table (`B + 0x274`, stride `0x1c`) and
set a flag word at slot offset `0x18`: `0204d1e8`, `0204d22c`, `0204d270`,
`0204d2b4`, `0204d0e0`, `0204d12c`, `0204d2f8`, `0204cb44`. Three levers
carry the set; each one was a wall in §6 before it was found.

**10.1 A live callee result keeps `r0` occupied.** Retail's temp trio starts
at `r1`, not `r0`, whenever the helper's return value survives to the
epilogue. Caching the call in a local and returning it makes the allocator
keep `r0` live and shifts everything up one register:

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

Dropping the local (`func(...);` then the RMW) starts the trio at `r0` and
the unit misses by one register — exactly the §6 "one-register-low" class.

The lever is not tied to the flag RMW: `0204d178` and `0204d1b0` are the same
shapes with a single `*(u8 *)(t + a0 * 0x1c + 2) = a2;` instead of the OR, and
they also need the live result (`mov r1, #0x1c` rather than `r0`).

**10.2 `p = t; p += i;` materialises an address C would otherwise fold.**
Retail emits `add rB, rT, rI` and then offsets from `rB`. Writing `p = t + i`
as one expression lets the compiler pick `mla` plus a folded immediate, which
is one instruction shorter. Assigning the base and then incrementing keeps
the index live for a second use, so the address is really materialised:

```c
    i = a0 * 0x1c;
    t = B + 0x274;
    p = t;
    p += i;                        /* add rB, rT, rI, no mla */
    if (a2 == *(u8 *)(p + 2)) ...
    if (a1 == *(u8 *)(t + i)) ...  /* second use keeps both live */
```

The same function also wants the index computed *before* the base is loaded,
which only happens once the pointer increment is spelled out.

**10.3 An inline loop bound is re-read every iteration.** Retail reloads the
count (`ldrh r0, [sb, #2]`) at the loop tail. Caching it in a local hoists a
single load and the loop shrinks by 8 bytes:

```c
    for (i = 0; i < *(u16 *)(a0 + 2); i++, a1 += 0x10) { ... }
```

`func_ov002_0204c824` is the residue of this family: it wants `mla r0, ...`
(the slot pointer in `r0`) followed by `add r3, r0, #0x18`, while C puts the
slot pointer in `r3` and folds `0x18` into the load/store. Swept: pointer
local, `u32 *p[6]`, `f = p; f += 6;`, split `*f = *f & ~0x400`, separate
base and slot pointers. Same materialisation wall as §6.

## 11. Solved: the ov003/ov005 stub and thunk set (54 functions)

Whole modules of small units are reachable in one pass once the shapes are
recognised. Fifty-four units went in together; none needed a lever beyond
writing the obvious C.

**11.1 `mov r0, #0; bx lr` is `return 0;`.** 23 ov003/ov005 functions are
8-byte constant returns. A `void` body compiles to a bare `bx lr`, so the
return type has to be non-void for the `mov` to appear.

```c
u32 func(void)
{
    return 0;
}
```

**11.2 `ldr ip, =callee` plus argument moves is a tail call.** `mov r0, #1;
mov r2, r0; mov r1, #0; bx ip` is exactly `return callee(1, 0, 1);`. A plain
last-statement call tail-calls too, but returning the result keeps the `u32`
prototype honest and picks up the value form of the thunk.

**11.3 A small table index in front of a thunk has to stay scaled.**
`func_ov003_020641c8` loads `data_ov003_020dc744[a1]` and passes it as the
third argument; the `<< 2` must be written on the index, as in the sibling
`func_ov003_020641e4`, or the compiler folds the scale into the load and the
argument order changes.

**11.4 A `== 0` test picking between `sub` and `rsb` is a predicated pair.**
`func_ov003_020555b4` compares the selector against zero once and emits both
arms predicated; write the `== 0` arm first (`(a1 << 5) - 0x1000`), then the
`else` arm as `0x1000 - (a1 << 5)` so it becomes the `rsb`.

## 8. Prior art

Levers above are cross-checked against, and reusable from:
`tangosdev/sm64ds-decomp` `notes/mwccarm-codegen.md` (statement order colours
temps; the materialisation wall and its build dependence; struct-copy load
batching), `fuddlesworth/pokebw2` `docs/matching.md` (narrow parameters change
caller scheduling; direct-global accesses instead of cached; pointer-reference
count flips added-vs-folded), `ReGame-Labs/dwds_decomp` (1.2/sp4 `-O4,p`),
`6elphegor/acww-decomp` (1.2/base, Thumb), `InusualZ/mhtri-decomp`
(subscript vs pointer-sum operand order), and the `decomp.wtf` MWCC guide.
