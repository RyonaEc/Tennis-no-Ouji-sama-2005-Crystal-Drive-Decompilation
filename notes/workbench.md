# Runtime evidence: the melonDS decomp workbench

[melonDS-with-decomp](https://hmatt1.github.io/melonDS-with-decomp/) is a melonDS
fork that records what the game does to every byte (coverage, call edges, write
history, data provenance) and exposes it through a local JSON-RPC API. This note
records the setup that was verified on 2026-11-22, what it answers that the
digest and the static tools in `tools/match/` cannot, and what it does not
replace. It is a second opinion on a unit, never a replacement for
`ninja sha1`.

## Setup

| | |
|---|---|
| Workbench | `M:\melonDS-workbench`, `melonDS-decomp-workbench-windows-x64.exe`, API 1.1 |
| ROM | `Tennis no Ouji-sama 2005 - Crystal Drive (Decrypted).nds`, SHA-1 `9eec3248d3569eba8abcb3244adcbdf01a5ea424` — **the same bytes the digest pins**, so a record is evidence about the code we rebuild |
| Launch | `melonDS-decomp-workbench-windows-x64.exe -w --api --write-history --data-origin <rom>` |
| JIT | off (`[JIT] Enable = false`). Recording stays off while the JIT is on, so this is not optional |
| Client | `dswb` from `tools/workbench/python` in the workbench source, unpacked to `M:\melonDS-workbench\pydswb\dswb` |
| Knowledge store | `workbench/ATOJ_8d555a80db47/knowledge.db` |

Two recording sessions were saved from the unlocked save and are named for it:
`sessions/unlocked-20261122-183931_f0-35338.dswbs` (35,338 frames) and
`sessions/unlocked-20261122-185046_f0-75818.dswbs` (75,818 frames).

The digest match is the point: the workbench keys its store on the ROM's SHA-1,
and it was given the decrypted revision, so nothing has to be re-derived from a
different dump.

## Loading our own names

`kb.import.static_analysis` takes a JSON path on the emulator side, in the shape
the README documents (`functions`, `labels`). Built from
`config/ATOJ/arm9/symbols.txt` it imported **1,365 functions and 457 labels**
in one call, so its disassembly, call graph and coverage panels show our
`func_0204xxxx` / `data_020xxxxx` names instead of `sub_`.

Only **arm9** is imported. All eleven overlays load at the same base
(`0x0204c7c0`), so an overlay address is ambiguous across modules and one
import would overwrite another; the workbench's own overlay table is the
authority for which one is resident. Its table agrees with our config exactly:

| overlay | RAM range | size | bss |
|---|---|---|---|
| `ov000` | `0x0204c7c0`..`0x0204c7e0` | `0x20` | – |
| `ov001` | `0x0204c7c0`..`0x02051ce0` | `0x5500` | `0x20` |
| `ov002` | `0x0204c7c0`..`0x02103920` | `0xb42c0` | `0x2ea0` |
| `ov003` | `0x0204c7c0`..`0x020eb840` | `0x9ef60` | `0x120` |
| `ov004` | `0x0204c7c0`..`0x020690c0` | `0x1c8e0` | `0x20` |
| `ov005` | `0x0204c7c0`..`0x020c9840` | `0x7cf80` | `0x100` |
| `ov006` | `0x0204c7c0`..`0x02077e80` | `0x2b680` | `0x40` |
| `ov007` | `0x0204c7c0`..`0x0204c7e0` | `0x20` | – |
| `ov008` | `0x0204c7c0`..`0x0204f460` | `0x2c80` | `0x20` |
| `ov009` | `0x0204c7c0`..`0x02054fa0` | `0x87c0` | `0x20` |
| `ov010` | `0x0204c7c0`..`0x0205e520` | `0x11d40` | `0x20` |

None is compressed. `ov004`'s `0x1c8e0` is exactly the size of
`extract/ATOJ/arm9_overlays/ov004.bin`, which is what makes the string-bank
offsets in [`reference-docs.md`](reference-docs.md) usable as RAM addresses
while that overlay is resident.

## What it answers that the digest cannot

`ninja sha1` says a unit is right. It cannot say a unit *is*, and static
analysis cannot see through pointers. Observed, over a 35,375-frame session
(boot, title, menus, a match):

| address | executed | observed |
|---|---|---|
| `0x02026a04` (open, 76 B block fill) | frame 4 | 10,707 calls, 10,699 of them from `func_ov001_0204fb34` at `0x0204fb5c`, plus single calls from six ARM9 boot paths (`0x02005300`, `0x02006640`, `0x0200eac4`, …) |
| `0x0202c36c` (parked) | **never** | the one edge reported is the neighbouring `bl 0x0202c2a8` at `0x02025d14`; `0x0202c2a8` itself ran at frame 4 and ends exactly at `0x0202c36c` |
| `0x020124e0` (parked) | **never** | no callers, no callees |
| `0x020266c0` (parked) | frame 10 | 14 calls, all from `func_0202d5ec` |
| `0x0202cfa4` (parked) | frame 4 | entered with **no observed call edge** |

Two of the four parked walls never run in ordinary play. That does not make them
wrong — they are byte-exact requirements either way — but it means runtime
evidence will not arrive by playing more, and that a plausible shape cannot be
confirmed by watching the game.

The call site of `0x02026a04` is the useful kind of evidence. Disassembled, it
sets `r0 = 0xC0`, `r1 = [0x02051cc0] + 0x134c`, `r2 = 0x400` immediately before
the call, which agrees with what `tools/match/walls.txt` already recorded from
the source side — a **word** fill, `f(0xc0, base + 0x134c, 0x400)`. Anyone
starting that unit from a byte-memset shape is starting from the wrong signature.

**Provenance** answers a different question: where a RAM address came from. On
the resident overlay base it reports a card read of `overlay9_0001.bin`, ROM
`0x43400`, 21,504 bytes, frame 13, loader PC `0x0202bfc0` — i.e. the overlay
loader, and the file-to-RAM mapping, without guessing. The same call on a data
buffer answers "which file is this, and at what offset", which is how a RAM
table can be matched back to a static file.

## The API, in the shape that was actually used

```py
import sys; sys.path.insert(0, r"M:\melonDS-workbench\pydswb")
from dswb import Client

c = Client.connect(agent="buffy")            # reads workbench/api.json
c.status_get()                                # recording on? frames? history use?
c.metadata_get(0x0202C36C, size=4, group="instruction")   # first_exec: did it run?
c.calls_get(0x02026A04)                       # callers/callees with counts and frames
c.xrefs_get(0x02026A04)                       # same, as the Cross-references panel
c.writes_list(addr=0x020CDAE6, size=2)        # who wrote a field, with old/new values
c.origin_get(0x0204C7C0, size=8)              # overlay file, ROM offset, loader PC
c.disasm_get(0x0204fb34, count=20)            # the instructions around a call
c.coverage_get(addr=0x0204C7C0, size=0x1C000, kind="exec")   # what ran
c.overlays_list()                             # overlay table + load/evict events
c.kb_import_static_analysis(path=r"M:\melonDS-workbench\import.json")
c.recording_configure(save_session=True)      # write a .dswbs without stopping
```

`metadata_get(...).first_exec == 0` is the cheap "did this code ever run"
question, and it needs no write history. `writes_list` is the one to reach for
when a struct field's width or writer is the blocker; `origin_get` when the
question is which file a buffer is.

## What it does not do

- It does not produce source, and it does not gate a unit: the digest does that.
- Names go into its own `knowledge.db`, not into `symbols.txt`. Anything worth
  keeping has to be copied across by hand, under `docs/NAMING.md`'s rules.
- Its `Build > Compare` loop needs objdiff-cli and wibo and compares objects;
  our `ninja sha1` already links and checks the whole ROM, so that panel adds
  nothing here.
- Write history is capped (4,000,000 entries in memory by default, which this
  session reached and wrapped at frame 18,245); the Recording panel can move it
  to a file. Coverage and call edges do not depend on it.
- Recording needs the interpreter, so the game runs slower than normal melonDS.

## Two saves, two jobs

The game uses a single save file next to the ROM and reads it on boot, so the
profile is chosen by renaming that file, not by a menu:

| save | what it answers |
|---|---|
| unlocked (the Action Replay-seeded `.sav` in that folder) | breadth. Every mode, character and court is reachable, so one session covers far more code, and it runs the "existing backup on boot" path |
| new game (no `.sav` present) | progression. Unlock decisions and the flags they set, first-run defaults, save creation, intro and name entry — with a finished save none of that code runs, and a flag can be seen read but never seen written |

To record the new-game path, move the unlocked `.sav` aside, boot, record, then
rename it back. Name each session for the save it used (`unlocked-...`,
`newgame-...`): the session file is the only place that distinction survives,
and "who wrote this flag" is unanswerable from the unlocked recordings.

## Caveats of this particular save

The save in the workbench directory is a completed, Action Replay-seeded file.
That does not taint call graphs or coverage of the code that runs, but:

- data origin for anything the save brought in is lost when the state loads
  (the emulator cannot know where loaded data came from);
- an all-unlocked save reaches modes and events that a fresh save would gate, so
  coverage from it is *generous*, not representative — good for finding code
  that runs, useless for judging what a normal playthrough reaches.
