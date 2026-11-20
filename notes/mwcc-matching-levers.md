# MWCC matching levers (from pokebw2 / aletheia)

Distilled from two 100%-matching NDS decomps, keeping only what applies to this
ROM. Source docs: `fuddlesworth/pokebw2` `docs/matching.md` and
`docs/decompiling.md`, `Project-Holon/aletheia` `README.md`.

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
| `tools/decomp/compiler_probe.py` | compiles one file with many mwcc versions and reports MATCH/bytes per function; needs more compilers than `1.2/sp2` on disk (mw-executables provides `ds/`, `dsi/` builds) |
| `tools/decomp/locals.py` | prints the register/stack slot of every parameter and local, from the object's debug info — would settle the register-tie walls directly, but needs `-g` in our flags |
| `tools/decomp/try_variants.py --score` | applies `=====`-separated variants and scores aligned diff lines — our `near.py` without the scoring |
| `tools/decomp/rodata_solver.py` | anneals declaration order until the data layout is forced |
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
