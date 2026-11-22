# MWCC matching levers (from pokebw2 / aletheia)

Distilled from two 100%-matching NDS decomps, keeping only what applies to this
ROM. Source docs: `fuddlesworth/pokebw2` `pokebw2/docs/matching.md` and
`pokebw2/docs/decompiling.md`, `Project-Holon/aletheia` `README.md`.

## Parameters and stack slots

- **Taking a parameter's address makes MWCC push every argument register** to
  give it a home. An original that pushes fewer copied the parameter to a local
  first and passed *that* local's address.
- **A by-value parameter the original keeps in its stack slot and reloads at
  every use, even with no call between, is `volatile` where it is declared.**
  `volatile` on a *local copy* does not do this.
- A parameter stored at entry into an outgoing argument slot and read back later,
  in a frame with no slot of its own for it, is that parameter's spill — not an
  argument of the call.
- **A `u64` parameter after three `u32`s splits between `r3` and the first stack
  word, with no register-pair alignment.** `(x, 0)`-style argument pairs and a
  callee saving `r3` plus its first stack arg to adjacent slots are the tells.
- Spilled parameters follow the rule at both ends of a call: a plain `u32`
  reloads *before* the call's stack argument stores, a spilled one after.
- Stack slots are allocated **top down**: the function's locals, then an `if`
  block's, then its `else` block's, each in declaration order, then inlines' in
  reverse. A frame slot no instruction touches can be a struct an inline
  returned by value.

## `const` and types

- A **`const` pointer parameter** lets MWCC keep a value loaded through it across
  a store through another pointer. When loads follow a call and the original
  stores the stack arguments *last*, the pointer is `const`.
- A `const` **by-value** parameter stays in a register even when its address is
  passed on. A `const` *local copy* of a plain parameter does not schedule like
  a `const` parameter.
- A parameter cast to a block-scoped typed pointer where it is used
  (`SOCSockAddrIn *addr = from;`) allocates differently from a typed parameter.
- Narrow types matter: a caller's narrow parameter converted to the callee's
  `u32` counts, and it moves the stack-argument stores.

## Locals, pointers, order

- Register allocation follows **declaration order of locals** — reorder before
  rethinking the algorithm.
- `a = x; a += y;` can fix a load order that `a = x + y` leaves to MWCC.
- A **local pointer to a struct member** (`PrintWindow *w = &work->priceWindow;`)
  is kept as the member offset in a callee-saved register and added to the base
  at each use; *where* it is assigned decides when that register loads.
- A parameter reused as a loop counter is allocated apart from a local counter,
  even when the parameter is otherwise dead.
- A loop that walks on from a parameter reuses the parameter as its cursor; the
  start kept in a local (`int start = pos;`) is what keeps them apart.
- A field read with nothing that can change it between uses is loaded once, into
  a value allocated *after* the declared locals.

## Tells for "this is not the same compiler"

- The game may not have been built with one compiler. Both projects found
  library code built by other versions (Black: `2.0/sp2p2` and `1.2`; aletheia
  lists `1.2/sp2` among its per-function versions). Try other versions on
  **library** code only — and note `1.2` needs flags **without `-ipa file`**.
- `dsi/1.1` differs from later builds by reusing a stored register where the
  game reloads the field (`strb r0,[r4,r7]; ldrb r0,[r4,r7]`). That reload is
  the compiler, not `volatile`.
- A version mismatch shows up as register-level differences, not structure.

## Rules that keep matching honest

- **Verify branch destinations and return values before blaming register
  allocation** — a "wrong register" is often a wrong constant (they found
  `GX_WNDMASK_W0` where the source said `OW`).
- MWCC allocates registers and stack slots **before** merging identical tails:
  one call written in both branches counts twice for the allocator.
- A missing variadic argument changes registers without changing the size; read
  the ROM's format string.
- Relocation masks include call targets, so a masked-byte MATCH never proves
  which function a call reaches.

## Tools worth porting later

| pokebw2 tool | Use here |
|---|---|
| `pokebw2/tools/decomp/compiler_probe.py` | compiles one file with many mwcc versions and reports MATCH/bytes per function; needs more compilers than `1.2/sp2` on disk (mw-executables provides `ds/`, `dsi/` builds) |
| `pokebw2/tools/decomp/locals.py` | prints the register/stack slot of every parameter and local from the object's debug info — **already ported** as `tools/match/slots.py`; `-sym on` is in our flags, so nothing to add |
| `pokebw2/tools/decomp/try_variants.py --score` | applies `=====`-separated variants and scores aligned diff lines — our `near.py` without the scoring |
| `pokebw2/tools/decomp/rodata_solver.py` | anneals declaration order until the data layout is forced |
- **Hoist a computed base before an early-return guard, then advance it with
  `+=`.** Two spellings of the same address generate different code:
  `p = b + OFF; if (b[k] == 0) return 0; p += a;` materialises `b + OFF` into
  a register *before* the guard compare and then does a single `add` for `a`,
  while `p = b + OFF + a;` after the guard folds the constant into an
  `add r0, r0, #0x4000` / `ldrb [r0, #0xbc0]` pair. The `+=` form is what
  retail used in `func_ov002_020b8750` (100 B, matched 2026-11-20).
- **A `u16` loop counter must be initialised before the last guard, not at the
  top.** In `func_ov003_0205578c` (72 B, matched 2026-11-20) retail emits
  `cmp r2,#0` / `mov r3,#0` / `ble`, i.e. the `i = 0` sits *between* the guard's
  compare and its branch. Writing `i = 0;` after `if (n <= 0) return;` puts the
  `mov` after the `ble` and is 4 bytes longer. The matching order is:
  `acc = K; if (...) goto end; n = a + 1; i = 0; if (n <= 0) goto end; do {...}
  while (i < n); end: return acc;` — with `goto end` so both guards share one
  epilogue, and a `do/while` because retail tests at the loop head.
- **Unsigned arguments turn signed division fixups into plain shifts.** A
  `s32` operand dividing by 2/4 emits `asr`/`add`/`lsr` rounding sequences; make
  the parameter `u32` and the same `/ 2` becomes a single `lsr #1`, and `/ 60`
  becomes `umull` by `0x88888889` + `lsr #5`. Two bytes per division differ.
- **A data symbol is either an array or a pointer, and the load count tells you
  which.** `extern u8 data_x[]` used as `*(s16 *)(data_x + 0x14)` produces one
  pooled `ldr` (address as base); declaring it `extern u8 *data_x` adds a second
  `ldr` to dereference. If retail has exactly one `ldr rN, [pc, #..]` before
  using the address, the symbol is an array.
- **A 12-byte gap can be one address, not one statement.** When an offset is
  not encodable as an ARM immediate, mwcc either pools the whole offset
  (`ldr rN, =0x8736` + `add`) or splits it at the nearest encodable base
  (`add rN, g, #0x8700` + `[rN, #0x36]`). Both are two instructions and both
  are correct code. Hoisting the pointer *before* a guard forces the pool form
  but keeps it live across the guard, which can add a frame
  (`push {r4, lr}`, +4 bytes). See `func_ov003_020960a4`.
- **Reusing a constant register where retail reloads it is a one-instruction
  tie.** `if (r >= 0x3b9ac9ff) return 0x3b9ac9ff;` gives mwcc `cmp r0, r2` then
  `movge r0, r2`; retail emits `cmp r0, r2` then `ldrge r0, [pc, #..]` from the
  same pool slot. The constant is rematerialised rather than copied. Spelling
  the limit as `>`-form, a named variable or an `if/else` all keep the copy.
  See `func_02008178`: 96/96 bytes, one instruction apart.
- **mwcc folds field offsets into the addressing mode; retail often does not.**
  Given `p = g + 0x1824; q = p + 0xcc; *q = *q + 1;` mwcc emits
  `ldr r0, [r2, #0xcc]` / `str r0, [r2, #0xcc]`, while retail materialises the
  full address once (`add r1, r2, #0xcc`) and indexes `[r1]` three times. The
  folded form is *shorter*, so a 4-byte gap here is not a missing statement.
  Pointer variables, `u32` array indices (`q[0x33]`) and two-step bases
  (`p + 0xb8`) all fold. Distinct from the pool-versus-split case, where both
  forms are the same length. See `func_ov002_0204f898`,
  `func_ov002_02052608`.
- **A `u8` local flips the `cmp` operand order.** When retail compares a
  freshly loaded byte against a parameter and mwcc emits the parameter first
  (`cmp r6, r0`), loading the byte into a `u8` local and comparing
  `if (v != a0)` restores retail's `cmp r0, r6`. Confirmed on
  `func_020145b8` (96 B, matched 2026-11-20). Writing the loaded expression
  inline gives the wrong order regardless of `!=` or `==` spelling.

## See also

[`ds-prior-art.md`](ds-prior-art.md) lists the other DS matching decomps and
which of their documents answers which question. This file stays a list of
levers; the evidence behind each row is in
[`mwccarm-codegen-log.md`](mwccarm-codegen-log.md).
