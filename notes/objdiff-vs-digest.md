# Why a digest-verified unit can still show 99.8% fuzzy in objdiff

Two ov004 units installed from Delta (`func_ov004_0205a63c`, 116 B and
`func_ov004_02051148`, 120 B) link to the exact target ROM
(`8d555a80...`, `ninja sha1` prints `OK`) and pass `t10 --check`, yet the
objdiff report scores them `fuzzy_match_percent: 99.83` with
`complete_code_percent: 100.0`. `build/progress.py` credits a function only at
100% fuzzy, so they add nothing to `PROGRESS.md`/`README.md`.

I disassembled the compiled object against retail to see which bytes differ:

| unit | instructions | differing | what differs |
|---|---|---|---|
| `0x0205a63c` | 29 / 29 | **0** | nothing at instruction level |
| `0x02051148` | 30 / 30 | 1 | `bl` target only: retail `0x02051448`, object `bl self+0x48` |

The single "difference" is an unresolved call relocation: the object holds a
`bl` with a zero offset (disassembles as `function + 0x48`), and the linker
fills the real target. `build/var3.py` masks relocation bytes by design, which
is why the gate accepted both units; objdiff compares relocatable objects and
counts those bytes.

So `99.83` is an **object-comparison artifact, not a partial match**. The ROM
digest is the authority (`AGENTS.md`), and both units are correctly installed.

Consequence to keep in mind: objdiff-based totals under-count by any unit whose
only residual is a relocation or literal-pool byte. The report's own
`complete_units` aggregate read 1,317 while `progress.py` wrote 1,301 on the same
data, so the two rules already disagree by ~16 units project-wide. Do not hand-
edit the README to close the gap; if it matters, change the counting rule in
`progress.py` deliberately and regenerate everything.
