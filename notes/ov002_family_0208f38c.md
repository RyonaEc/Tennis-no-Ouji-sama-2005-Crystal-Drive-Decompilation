# ov002 family 0x0208f38c (14 members, 608 B each)

Groundwork for the largest remaining ov002 group (8,512 B). Written from a full
disassembly of the representative; no source yet. Members: 0x0208f38c,
0x02096a64, 0x02098aac, 0x020a06e8, 0x020a42dc, 0x020a47f8 and eight more (see
`python tools/match/famsurvey.py ov002`).

## Shape

A per-entity per-frame update: nine-register push, `0x3c` stack frame, one index
argument, no return value. The body builds a small argument block on the stack,
calls six helpers and finally clears one flag bit.

## Data

- `a0` is a record index; the record stride is `0x158` (`mul r4, r0, #0x158`),
  and the index is kept at `[sp+0x20]` for later use.
- `g = *(u32 *)<pool>` is the record base; the record pointer is `g + a0*0x158`.
- `g + 0x2e4` is a byte/word array; `g + a0*0x158 + 0x2e6` is a byte whose low bit
  drives a sign (`lsr #1`, `lsl #1`, `sub #1`, `rsb`) and whose `>>1 & 0xff` is kept
  in r7 -- r7 is compared with 1 later, so it is a 0/1-style mode selector.
- A second table off `g` uses a `0x38` stride (`mla r1, a0, 0x38, base`), and its
  `+0x4c` word is tested for bit 2 (`ands #4`) while the parallel path tests the
  halfword at `sl+0x50` for bits `0x10` and `0x20`. Both are input-direction bits
  that adjust a value by adding or subtracting another.
- `sl = g + 0x2e4 + a0*0x158` is the per-record sub-object; fields used: `sl`,
  `sl+0x4c`, `sl+0x50` (u16), `sl+0x128`, `sl+0x100+0x2c` (u16).
- `sb` is a third table base whose `[sb]`, `[sb+8]` words feed clamps.

## Arithmetic

- Division by 5 appears as `smull` with a pooled magic then `asr #1` plus sign fixup.
- Division by 360 appears as the `0xb60b60b7` magic with `add ..., lsl #8` then `asr #8`.
- `0x200000` and `0x320000`, `0x30000`, `0x8000000` are position/scale constants;
  `rsb r2, r2, #0x30000` is a clamp of a stored word against `0x30000`.
- Two sign flips: `mvn r0, #0; mul` against `[sp+0x28]` and against `r4`.

## Calls, in order

| callee | arguments |
|---|---|
| `func_0205bf64` | none visible; result minus `0x14` becomes a value |
| `func_02053468` | none visible; result is `(s16)(r0 << 16 >> 16)` |
| `func_0205bee4` | `0x14`, `sl` |
| `func_0204f770` | `sp+0x2c` (out), plus two computed values |
| `func_020532e4` | `r4`, `[sp+0x28]`, `sp+0x2c`, value, `r6` |
| `func_0203b0c0` | two 16-bit values (the same routine used by 0x02082220/0x020c806c) |
| `func_020528cc` | `sp+0x2c`, `0x320000`, `0` |
| `func_02057068` | six words from the stack block |
| `func_020780f8` | none visible |

## Tail

`add r1, r8, #0x38; ldr r0, [r1]; bic r0, r0, #1; str r0, [r1]` clears bit 0 of the
word at `g+0x38`, then the frame is torn down.

## Why this is not a quick match

Byte-exactness here depends on eight callee signatures and three table layouts, so
it needs the workbench (see `notes/workbench.md`) to establish field meanings and
call arguments rather than further disassembly. The family is still the single
biggest win available in ov002.
