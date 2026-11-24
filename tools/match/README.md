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

**Sweeps, families and queues**

`autorun.py`, `resweep.py`, `sweep_tiny.py`, `sweep_homed.py` (generated
candidates), `sweep.py` (formulation sweep; `--compiler` takes a version tag), `clone.py`, `clone_match.py`,
`twins.py`, `twin.py`, `crosstwin.py`, `near.py` (clone a matched sibling),
`famsurvey.py`, `band.py`, `fam.py`, `family_asm.py`, `family_fill.py`,
`trivgen.py`, `synth.py`, `scan.py`, `census.py`, `jt.py`, `jtgen.py`,
`jtrun.py`, `dump_emit.py`, `newcand.py`, `ready.py`, `queue.py`.

Tracked rosters: `walls.txt` (one line per parked unit: address, size, symbol,
residue, and the line its note starts at in `walls-detail.txt`; read one unit
with `walls.py show <mod> <addr>`, never load the detail file whole),
`true_fresh.txt` (the unmatched queue), `tried.txt` (`autorun.py`'s memory),
`jt.txt` (the jump-table scan `jtrun.py` reads).
