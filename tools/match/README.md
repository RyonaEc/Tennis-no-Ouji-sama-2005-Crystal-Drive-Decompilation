# Matching tools

Helpers for finding, writing and installing byte-exact C. Run them from the
repository root; they read `config/`, `src/` and `extract/`, and write scratch
output under `build/` (gitignored).

**Harness and single unit**

| Script | Purpose |
|---|---|
| `fastmatch.py` | Shared compiler harness: compile one body, diff it against retail |
| `sdk.py` | Find a retail function's bytes in the prebuilt NitroSDK libraries (`build/cand/sdk`), so an SDK routine is identified instead of parked as a codegen wall |
| `sdkmap.py` | `sdkmap.py <mod>` maps every delinked function of a module onto the SDK libraries and writes `build/sdkmap-<mod>.tsv` (see `notes/nitrosdk.md`) |
| `census.py` | `census.py [--all] [--module M] [--write FILE]` buckets the units the generator cannot express by what their bytes are (zero-fill, data, veneer, stubs, no-return, multi-body, body, body+pool, body+code), with pool words identified from the body's own pc-relative loads; writes a TSV with `--write` |
| `walls.py` | `show <mod> <addr>`, `grep <text>`, `list --mod <mod>`, `add <mod> <addr> "<reason>"`: the wall log without loading the detail file |
| `udis.py` | Disassemble retail functions with their sizes |
| `var3.py` | Compile a candidate and diff it, relocations masked |
| `probe.py`, `vprobe.py` | Side-by-side listing; `vprobe` runs `1.2/b56` then `1.2/sp2` |
| `permute.py`, `pragmas.py`, `slots.py` | Statement order, pragma and register-slot experiments |
| `lint_cand.py` | Check a candidate's extern names against `symbols.txt` |

**Data units**

| Script | Purpose |
|---|---|
| `carve_data.py` | `list <mod> [lo] [hi]` writes the unclaimed `.rodata`/`.data` ranges as byte arrays; `accept <mod> [budget] [step]` keeps the groups whose addition still reproduces the digest, bisecting a failing group down to the one range that breaks it |
| `symreloc.py` | `scan <mod>...` reports the literal address words in existing data units whose relocation target is named, in this module or another one; `apply <mod>... [budget]` rewrites them as symbol references, keeps only the units whose module still reproduces the digest, and restores the rest. Only word-aligned `u32` units, and only words whose stored value is exactly the relocation target |

**Runtime evidence**

| Script | Purpose |
|---|---|
| `coverage.py` | ask the running melonDS workbench which unmatched functions ever executed, per module and largest-first (see `notes/workbench.md`) |

**Installing and verifying**

| Script | Purpose |
|---|---|
| `t10.py` | The ROM-digest gate (`--check`, `--repair`) |
| `reg.py` | Install a candidate through `t10.try_unit` |
| `verify_rollback.py` | Exercise `t10`'s match, miss and crash rollback paths |
| `progress.py` | Regenerate `PROGRESS.md` from the objdiff report |
| `check_bytes.py`, `compile_scan.py`, `cdiff.py`, `why_miss_c.py`, `diag_unit.py` | Compile-only diagnostics |

`t10.try_unit` byte-compares every candidate (and every variant) against retail
before it builds, and only the survivors are linked and digested. The compare is
sound as a *rejection*: it masks the words the linker fills, so it can accept a
body the digest later rejects, but a body with a differing unmasked byte cannot
link to retail. That makes a rejected variant cost ~0.16 s instead of a ~9 s
relink + ROM build, which matters for the sweeps. The digest is still the only
thing that accepts a unit. `try_unit` builds through `ninja rom` (not the
`rom`+`objdiff` default), and a re-test whose delinks entry already exists does
not re-run `configure.py`.

Functions that start at a halfword address (4n+2) used to be unlinkable: mwcc
and dsd both declare `sh_addralign` 4 for code, and the linker script's
`ALIGNALL(4)` pads the chunk up to the next word, shifting everything after it
by two bytes. `tools/align_code.py` runs in `build.ninja` between the objects
and the link, dropping executable sections to alignment 2 and the `.arm9`
`ALIGNALL` with them, so the delinked sizes decide the layout the way retail
does. The ten `swi` stubs are matched that way.

**Sweeps, families and queues**

`autorun.py`, `resweep.py`, `sweep_tiny.py`, `sweep_homed.py` (generated
candidates), `sweep.py` (formulation sweep; `--compiler` takes a version tag), `clone.py`, `clone_match.py`,
`twins.py`, `twin.py`, `crosstwin.py`, `near.py` (clone a matched sibling),
`famsurvey.py`, `band.py`, `fam.py`, `family_asm.py`, `family_fill.py`,
`trivgen.py`, `synth.py`, `scan.py`, `census.py`, `jt.py`, `jtgen.py`,
`jtrun.py`, `dump_emit.py`, `newcand.py`, `ready.py`, `queue.py`, `fresh.py`
(the live unmatched queue: `symbols.txt` minus source-backed `delinks.txt`
ranges, and the only thing that should regenerate `true_fresh.txt`),
`annotate.py` (a body's instructions, pool literals, symbols and callee
prototypes in one pass) and `respell.py` (one candidate re-spelled over the
choices that decide register colouring, ranked by differing slots).

Tracked rosters: `walls.txt` (one line per parked unit: address, size, symbol,
residue, and the line its note starts at in `walls-detail.txt`; read one unit
with `walls.py show <mod> <addr>`, never load the detail file whole),
`true_fresh.txt` (the unmatched queue), `tried.txt` (`autorun.py`'s memory),
`jt.txt` (the jump-table scan `jtrun.py` reads).

`grind.py` is the unattended version of the same search: it walks the walls and
then the live queue, re-spells each unit's closest candidate (a stored
`build/cand` file, a clone of a matched sibling, or the generator's attempt),
scores variants through `fastmatch`, and installs a hit only through
`t10.try_unit`. Every finished unit is appended to
`build/grind-journal.jsonl`, so a restart after a crash or a kill skips what was
already exhausted, and the journal keeps each unit's best residue for the hand
pass that follows.

`--daemon` is the mode that runs without an agent watching: pass after pass with
a sleep between them, a heartbeat in `build/grind-status.json`, and a graceful
stop whenever `build/grind.stop` appears (it is removed once honoured, so
stopping never means killing a process by pattern). `--retry-slots N` spends a
pass on the journal's near-misses instead of the queue, cheapest residue first,
because a unit recorded at one or two differing slots is where a deeper sweep
pays. `--status` prints the heartbeat, the totals and the closest exhausted
units without starting anything. `--commit` commits each kept unit locally so an
unattended run's work is in git history rather than only in the working tree; it
never pushes and never edits `PROGRESS.md` or `README.md` - that release step is
still the hand pass. One instance per build tree.
