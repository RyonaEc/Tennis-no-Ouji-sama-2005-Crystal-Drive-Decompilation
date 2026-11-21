# Naming: the external annotation layer

Matching comes first. Symbols stay `func_<address>` while any byte of the ROM
is unmatched, and `AGENTS.md`'s "never rename existing symbols" still holds:
renaming a matched unit is a risk with no matching payoff, and the digest is the
only thing that decides whether a unit is right. What this document adds is the
part that is safe to do now — recording what a unit *is*, without touching the
tree — plus a scripted pass that has already been proven to lose nothing.

## The layer

`notes/naming/annotations.tsv`, tab separated, one row per symbol:

| column | meaning |
|---|---|
| `module` | `arm9` or `ovNNN` |
| `address` | the retail address the unit was matched at |
| `symbol` | the current `func_<address>` / `data_<address>` name |
| `proposed` | the name the evidence supports |
| `confidence` | `high` (semantics proven by a callee or a register file), `medium` (register/data-flow reading), `low` (structure only) |
| `evidence` | what in the matched source or in retail says this — table, register address, call chain |
| `struct_notes` | pointer to the layout fragment in `docs/NAMING.md#struct-notes`, or `none` |

Rows are added when a unit matches and the evidence is fresh, not in a later
sweep. A row is not a promise: `low` rows are placeholders that a real pass may
replace, and a row whose `proposed` equals `symbol` is simply unnamed.

## The pass

`tools/match/names.py` reads the table and rewrites, per row:

- `src/<mod>/<symbol>.c` → `src/<mod>/<proposed>.c`
- the delinks entry key
- the definition line in that module's `symbols.txt`
- every identifier occurrence in `src/**.c` and `include/**.h`
- the per-file key in `config/ATOJ/mwcc_versions.txt`, when the unit has one

It is a token/path substitution, so line endings and delinks order are
untouched, and the linked ROM must come out byte-identical. `--dry-run` reports,
`--verify` proves no old name survives, and `--apply` refuses a tree containing
`.git` unless `--allow-live-tree` is passed, so the live tree cannot be rewritten
by accident. Rows already renamed are skipped, which makes a second run a no-op.

## Rehearsal (2026-11-22)

Run on a mirror tree (`build/rename-rehearsal/root`: copies of `src`, `config`,
`include`, `tools`, `extract` and a copy of `build/ATOJ`, so the build was
incremental):

| step | result |
|---|---|
| `ninja sha1` before renaming | `8d555a80db47278475ffdfdf808fe80dbd7c48cf … OK` |
| `names.py --apply` (4 rows: `0x0204efa0`, `0x0204ebb0`, `0x0206d778`, `0x020ce31c`) | 17 rewrites |
| `tools/configure.py ATOJ && ninja sha1` | 10 steps, digest unchanged, `OK` |
| `t10.py --check arm9 ov001 ov003 ov005` | all `ok`, `dangling=0 unlinked=0 dupes=0` |
| `names.py --verify` | 0 stale/missing |
| second `--dry-run` | 0 rewrites, all rows `skip (already renamed)` |
| `diff` of each renamed source against the original | one line: the signature name |

So the rename is mechanical: nothing outside the five recorded places holds a
symbol name, and the compiler does not care what the file is called.

The rehearsal directory has been deleted; the proof is this table, and
`names.py --root <copy> --apply` reproduces it on any copy of the tree.

## What a real pass still has to handle

- `tools/match/tried.txt`, `tools/match/true_fresh.txt` and the `notes/` log use
  old names as queue bookkeeping and history. `names.py` deliberately leaves
  them alone: history should keep the name of its time, and the two rosters are
  derivable (`tried.txt`/`true_fresh.txt` are regenerated from the tree and the
  delinks).
- `PROGRESS.md` and the objdiff report key off delinks entries, so they follow
  automatically; `README.md` counts do not mention names.
- The pass should still be gated the way every unit is: `ninja sha1` must print
  the same digest, and that gate is the reason to do it in one commit rather
  than a unit at a time.

## Struct notes

Observations only, taken from offsets a *matching* unit already commits to.
No names are attached; only offset, width and the use that was proven.

### ov005 state block, base `data_ov005_020c9740`

| offset | width | seen as | evidence |
|---|---|---|---|
| +0x36 | u16 | mode/value written as 2 | `func_ov005_0206d778` true arm |
| +0x4330 | u16 | timer written as 0x44 | false arm |
| +0x4332 | u16 | zeroed | false arm |
| +0x4388 | u16 | zeroed | false arm |
| +0x43a6 | u16 | counter compared with 0x10, post-incremented | both arms |

One base pointer is held across the whole function, so this is a single global
object rather than two.

### ov003 shared record array, base `data_ov003_020eb720`

| offset | width | seen as | evidence |
|---|---|---|---|
| +0x9fe8 | 4 | array base, index `i` in 0x78..0x7b | `func_ov003_020ce31c` |
| stride | 0x1c | `i * 0x1c` | same |
| record +0x0 | u16 | passed as the second argument | `(u16)p[0]` |
| record +0x4 | u16 | passed as the third argument | `(u16)p[1]` |
| record +0x10 | ptr | passed as a u16 pointer | `p[4]` |

The array starts at index 0x78, so either the preceding 0x78 records are a
different array or the members before `+0x9fe8` belong to the same structure;
the matched source cannot tell, and guessing would be invention.

### ov008 8-byte slot record

| offset | width | seen as | evidence |
|---|---|---|---|
| +0x0 | u32 | filled with 0x200 for all 0x80 records | `func_ov008_0204e60c` |
| +0x4 | u16 | padding, never touched | same |
| +0x6 | u16 | filled with 0 | same |
| stride | 8 | `i * 8` over 0x80 records | same |

### ov004 instance table, base = the pointer in `func_ov004_02050154`'s first argument

| offset | width | seen as | evidence |
|---|---|---|---|
| +0x0 | s32 | element count, loop bound | `func_ov004_02050154` |
| +0xc | u32 | id compared against each object's +0x98 | same |
| +0x18 | u32 | handle written to the out parameter on a hit | same |
| +0x20 | ptr[] | array of object pointers, `i` scaled by 4 | same |
| object +0x98 | u32 | per-object id | same |

The same layout appears in the unmatched sibling `func_ov004_02050068`, which walks the
same table and calls the function pointer stored at table +0x8 when the ids match - so
the +0x8 slot is a callback, not data.

This one is not just a label: writing the same loop through a flat `u8 *`
produced the same 76 bytes except for the two hoisted constants and the address
temporary, and the member form reproduces retail's register assignment exactly
(`r2` = 0, `r3` = 0x200). The record layout is therefore part of the match, and
`docs/NAMING.md#struct-notes` is where it is kept.
