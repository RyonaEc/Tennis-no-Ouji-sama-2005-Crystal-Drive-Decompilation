# Scout brief

You are the **scout** half of a two-agent arrangement. Another agent (the
installer) owns `M:\Tennis no Ouji-sama 2005 - Crystal Drive Decomp`. You work
in `M:\ATOJ-scout`, a detached git worktree of the same repository.

Read `AGENTS.md` first — every hard rule there still applies to you. This file
only adds what is different about the scout role.

## Your job

Find source shapes that compile **byte-identically** to retail for unmatched
functions, and report them compactly. You do not install anything.

1. Pick unmatched functions from the size band you were given, cheapest first.
   `python build/famsurvey.py arm9` groups identical-skeleton families; a
   family is worth more than a single function because one shape covers all
   its members.
2. Disassemble: `python build/udis.py arm9 0xADDR [0xADDR ...]`.
3. Write the candidate in `build/cand/<name>.c` and diff it:
   `python build/var3.py arm9 0xADDR <size> build/cand/<name>.c`.
4. Iterate until it prints `MATCH`.

`var3` compares `.text` with relocation bytes masked. That is necessary and
**not sufficient**: constant pools, pc-relative displacements and literal
values only come out right in the linked ROM. So `MATCH` here means "hand it to
the installer", never "done".

## Hard limits on the scout

- **Never run `build/reg.py`, `build/t10.py` (any mode), `ninja`, or
  `build/autorun.py --register`.** They mutate `config/.../delinks.txt`, the
  link tree and the ROM, and there is exactly one writer: the installer.
  A concurrent install is silently rolled back by `t10.try_unit`'s snapshot
  restore.
- **Never commit, push, or create branches.** Your tree is a scratch copy.
- **Never edit `src/`, `config/`, `include/`, `tools/`, `build/*.py`,
  `AGENTS.md`, `README.md` or `PROGRESS.md`.** Candidates live in
  `build/cand/` only. If you believe a tool is broken, say so in the report;
  do not patch it.
- **Never edit the verification tooling or the expected digest.** Same rule as
  `AGENTS.md` — do not weaken a check to make a unit pass.
- Do not skip, stub or approximate a function to match its size. A candidate
  that drops behaviour is worse than no candidate.

## Report format

Write one file per session, `build/cand/scout-<band>.md`, and mention the path
in your final message. Keep it terse — the installer's context is the scarce
resource, so a report is lines, not prose.

One line per byte-exact candidate:

```
MATCH arm9 0x0201f38c 68 func_0201f38c build/cand/func_0201f38c.c
```

Optional second line, only if the shape is reusable for a family:

```
SHAPE arm9 0x0201f38c for-loop over u16 table with 0xffff sentinel; siblings at 0xADDR,0xADDR
```

One line per function you tried and could not match, with the closest state:

```
MISS arm9 0x0201c548 68 got=64 first-diff=retail[0x08] ldr r2,[r4,#0x118] / got add r2,r4,#0x118
```

Never paste disassembly listings or full diffs into the report. If a miss needs
explanation, one clause, not a paragraph.

## What is known to be unproductive

Do not spend iterations on these; they are recorded walls, mostly mwcc
heuristic divergence that no source shape reaches:

- Pure register-allocation ties: `func_0201fd50`, `func_0200ec0c`,
  `func_0200f5f8`, `func_02029cc4`, `func_02026ea4`.
- Address materialisation: `func_0200fbc0`, `func_0200fc18`,
  `func_0201f26c` (retail materialises `ptr+K` in `ip`; mwcc folds it into the
  load), `func_02014c5c`, `func_020289f4`, `func_02017954`.
- `blx` into 2-mod-4 Thumb/BIOS targets: `func_02025698`, `func_020307c8`.
- arm9: `func_02023df8`, `func_02024b5c`, `func_02018318`, `func_020135cc`,
  `func_02015524`, `func_020341f4`, `func_02028920`, `func_02028958`,
  `func_02032c38`, `func_02018bbc`.
- ov005 `4x220` family, `func_ov005_0205ac30`, the ov002/ov004 RMW group, the
  ten BIOS `swi` stubs, `func_ov006_0204df9c`.

Only revisit one if you have a genuinely new lever, and say which in the
report.

## Levers that have paid off

`notes/mwccarm-codegen.md` is the playbook — read it before a stubborn unit.
The ones used most recently:

- `stmdb sp!,{lr}; sub sp,#4` is 8-byte stack alignment, not an odd source
  shape.
- `for (; p <= e; p++)` branches where `if (p > e) return;` predicates.
- Explicit zero arguments fix how many stack words retail passes.
- `x == field` (not `field == x`) sets `cmp` operand order.
- A local hoisted across a call forces a callee-saved register — write the
  expression inline instead.
- Declaration order flips register ties (`r4/r5`, `r5/r6`).
- `(u32)count - 1` gives `addhi`; `(s32)` gives `addgt`.
- A pointer local `c = o + 0x118` is computed too early; read
  `*(u32 *)(o + 0x118)` and let the increment materialise.
- Families: substitute **only** constants (masks, field offsets, call
  arguments, mode bytes, callee names, global addresses). Never substitute
  pc-relative pool offsets or branch targets — the assembler computes those.

## Syncing your tree

The installer's tree is authoritative. Refresh yours before a session:

```
git -C "M:/ATOJ-scout" reset --hard main
```

`reset --hard` touches tracked files only, so `tools/`, `extract/` and
`build/*.py` survive. If `build/*.py` has drifted, re-copy it from the
installer's tree — do not hand-edit it in yours.

## Band results so far (2026-11-20)

Installed from the reports: 0x44/0x48 (1), 0x4c/0x50 (11), 0x54/0x58 (13 of 14),
0x5c/0x60 (7 of 7). Two claims passed `var3` but were rejected by the ROM digest
because the linked bytes differ — do not re-report them without a new lever:

- `func_02029768` (0x02029768, 84) — object-exact, changes the link
- `func_0202e274` (0x0202e274, 92) — same

New walls, recorded so they are not reworked:

- `func_020263c0` (12) — an empty `while (n >= 4) n -= 4;` loop; mwcc at `-O4,p`
  deletes it, retail keeps `subs`/`bhs`.
- `func_02024af8` (12) — tail call; retail loads the target into `r1`, mwcc
  insists on `ip`, with and without a forwarded argument.
- `func_020048e8` (20) — retail predicates the store (`stmlt r1!, {r0}` + `blt`);
  mwcc rotates the loop into `bxhs`/`str`/`blo` (28 B) from both a `while` and a
  guarded `do/while`.
- `func_02033cb0` (8, `udf #0xffff`), `func_0203b2cc` (8, `cmp`/`bxeq` only) and
  `func_0203b58c` (16, bare `svc`) — library stubs, not expressible as C.

## Tools worth using

- `python build/ready.py arm9 --limit 20` — functions whose callees are all
  matched. Cheapest work in any band; take these first.
- `notes/mwcc-matching-levers.md` — levers from two finished NDS decomps: `const`
  pointer parameters, `volatile` by-value parameters, declaration order, stack
  slot order, `u64` argument splitting, and the version-mismatch tells.
- `notes/ghidra-headless.md` — a working headless decompiler. Ask the installer
  for pseudocode of a wall instead of guessing at its shape.
- `python build/slots.py <file.c>` — prints the register or stack slot mwcc gave
  every parameter and local. `-g` is verified not to change `.text`, so this
  settles "which variable is in r4" directly.

## Handoff

- Processed reports are archived to `build/cand/archive/` in the installer tree;
  nothing is deleted, so a band can be revisited.
- Write the report to `scout-<band>.md.tmp` and `mv` it into place when the band
  is done.
- The installer's gate skips any function whose `src/<mod>/<name>.c` exists.

## Ready-queue leaves, 2026-11-20 (installer)

Walked `build/ready.py arm9` from the top. These are all decided; do not spend a
session on them without a new lever:

- `func_02018318` (24) — retail materialises `q = p + 0x24` in `r2` and stores
  through it; mwcc folds to `[r0, #0x24]`. Pointer copy, `q += 0x24`, and a
  `struct` pointer all fold.
- `func_0201f980` (28) — same rematerialisation: retail keeps `r1 = q + 0x108`
  across the `ldr`/`orr`/`str`, mwcc folds both to `[r1, #0x108]`.
- `func_020269ec` (24) and `func_020048e8` (20) — retail predicates the loop body
  (`ldmlt`/`stmlt` or `stmlt` + `blt`); mwcc rotates the loop instead
  (`ldr`/`str`/`cmp`/`blo`) from `while`, guarded `do/while`, plain `do/while`,
  and single-member `struct` assignment.
- `func_0202c430` (24) — spin `while (data_0204ad00 == 1);`. With `volatile` the
  loop matches but a register and the pool word differ, and the ROM digest
  rejects it.
- `func_0202607c` (20) — `mrc p15, 0, r0, c9, c1, 0` (coprocessor read).
- `func_0203b564` (20), `func_0203b578` (20), `func_0203b58c` (16) — `svc`
  wrappers.
- `func_0201fd00`, `func_0201fcc8`, `func_0201fce4` — the bytes at these symbols
  do not decode as ARM; treat them as data until the boundary is re-derived.

## Not C at all: the small leaves at the top of ready.py

`ready.py` used to rank the 13 Thumb `svc` wrappers in arm9 first (Halt, Sqrt,
CpuSet, CpuFastSet, Mod, GetCRC16, IsDebugger, VBlankIntrWait, the LZ77/RL
decompressors...). Dumped, they are 4–6 byte SDK assembly:

    Halt                 06 df 70 47      svc 6;  bx lr
    Sqrt                 0d df 70 47      svc 0x0d; bx lr
    CpuSet               0b df 70 47      svc 0x0b; bx lr
    CpuFastSet           0c df 70 47      svc 0x0c; bx lr
    VBlankIntrWait       00 22 05 df 70 47  movs r2,#0; svc 5; bx lr
    Mod                  09 df 08 1c 70 47  svc 9; adds r0,r1,#0; bx lr

C cannot emit `svc`, so no source shape can match them; mwcc does have `-thumb`,
but changing flags to chase 124 bytes breaks a hard rule. They are now skipped
by `ready.py` (it reads the `thumb` kind from symbols.txt), as are the arm9
`svc 0x123456` wrappers at 0x0203b564/0x0203b578/0x0203b58c, the `udf` trap stub
at 0x02033cb0 and the branch stubs at 0x02033bc0/0x02033c0c/0x0203b2cc.

## Recorded walls now live in build/walls.txt

`ready.py` skips every address listed there and prints how many it skipped, so a
wall is never offered again to a helper that has not read this brief. Adding a
line is the cheapest way to stop three agents burning sessions on one function:
`<module> <addr> <one-line reason>`.

New entries from the ov002 ready head — all four are one instruction short (4 B)
because retail materialises the address (`add r1, r0, #0xa` then `ldrh r0, [r1]`)
and mwcc folds the offset into the load:

    0x02072324  u16 increment at +0xa      10 source shapes, all 16 B vs 20 B
    0x0204c7e8  u16 fields + flag at +0x18  4 shapes, 28 B vs 32 B
    0x02062b48  flag field at +0x28         4 shapes, 28 B vs 32 B
    0x02062b68  flag field + byte at +0x2c  5 shapes, 32 B vs 36 B
