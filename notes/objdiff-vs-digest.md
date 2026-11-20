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

## Correction: two separate causes, and one was a real bug in progress.py

The artifact above is real for units whose measures carry `matched_*` and land at
99.83% fuzzy. But the larger gap had a different, fixable cause: objdiff omits
the `matched_code` / `matched_functions` keys entirely for some fully complete
units and emits only `complete_code` / `complete_units`. `progress.py` read
`m.get('matched_code', '0')` and `m.get('matched_functions', 0)`, so every such
unit scored **zero functions and zero bytes** despite `fuzzy_match_percent:
100.0` and a verified ROM digest.

At the time of writing that was **17 units, 17 functions, 1,076 bytes** —
including all four of Delta's ov004 matches and two ov003 units — invisible in
the headline counts. The fix is a fallback:

```python
code  = int(m.get('matched_code',  m.get('complete_code', '0')))
funcs = m.get('matched_functions', m.get('complete_units', 0))
```

`PROGRESS.md` then reads 1,318 functions / 83,732 bytes instead of 1,301 /
82,656. This is a reporting fix only: `build/t10.py` and `ninja sha1` remain the
authority, and no verification was weakened. `build/` is gitignored, so the
patched `progress.py` exists only in the working tree — recreate this change if
the tooling is ever regenerated.
