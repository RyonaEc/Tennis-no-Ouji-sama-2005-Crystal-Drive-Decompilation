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

## Resolved callee signatures (from already-matched sources)

- The global base is `data_ov002_02102070` (`extern u8 data_ov002_02102070[]`), and
  `*(u32 *)data_ov002_02102070` is the pointer to the main state struct. Every
  `ldr rX, [pc]` at the top of a family member loads that symbol, so
  `g = *(u32 *)data_ov002_02102070`; the same word appears in other unmatched ov002
  units (e.g. 0x0205fe04, 0x020882d4, 0x0207ff34), so they all index one structure.
- `func_ov002_0205bf64(void)` returns `s16`, specifically
  `*(s16 *)(g + 0x1e00 + 92)` -- a live angle. In the family body its result minus
  `0x14` becomes `r3`, so `0x14` is the same 20 that `func_0205bee4` is called with.
- `func_ov002_0204f770(s32 a0, u32 a1, s32 *a2)` is a sine/cosine table lookup:
  `k = a1 & 0xff`, `a2[0] = (t[k] * a0) >> 8`, `a2[1] = (t[k + 0x40] * a0) >> 8`,
  `a2[2] = 0`, with `t = data_ov002_020fc208` (`s16[]`, 0x80 entries). In the family
  the call is `func_0204f770(scaled, angle_byte, sp + 0x2c)`, so `sp + 0x2c` is a
  three-word sine/cosine result that the rest of the body then clamps and passes on.

## Reading of the body with those signatures

The routine computes an entity's heading (from the live angle and the per-record
0x38-stride table at `+0x8`), converts it to a byte angle by dividing by 360,
turns that into a scaled sine/cosine pair, applies direction bits from `sl+0x4c`
(bit 2) or `sl+0x50` (bits 0x10/0x20) to add or subtract a component, clamps the
result against `0x30000`, and finally feeds three words plus the mode to
`func_02057068`. That is a per-entity 3D placement, which is consistent with the
nine-register frame and the three table bases.

## `func_ov002_0205bee4` decoded (third callee resolved)

`func_ov002_0205bee4(s32 a0, u8 *a1)` scales `a0` by a factor derived from `a1`:

- `g = *(u32 *)data_ov002_02102070`, then `t = g + 0x274 + *(u8 *)(a1 + 2) * 0x1c`.
- If `*(u8 *)(a1 + 0xd1) == 0` and `(*(u8 *)(t + 2) + 0xd9) & 0xff` is 0 or 1, the
  factor source is 0; otherwise it is `*(u16 *)(a1 + 0xe8)`.
- `n = min((u16)source >> 1, 2)`; factor is `2 - n`; the result is
  `(s16)((a0 * (2 - n) + carry) / 2)`, where the `add rX, rX, rX lsr #31` before the
  final `lsl #0xf` / `asr #0x10` is the negative-rounding fixup for that halving.

So in the family body `func_0205bee4(0x14, sl)` yields "20 scaled by this entity's
speed/state factor", and the result is added to or subtracted from the sine/cosine
component depending on the direction bits. That is consistent with a movement
step of up to 20 units per frame, halved and scaled.

The remaining unresolved callees are `0x02053468` (0xdc), `0x020532e4` (0x184),
`0x020528cc` (0xa8), `0x02057068` (0xa34), `0x020780f8` (0xbc) and
`func_0203b0c0` (0x20c, main).

## Two more callees inspected (partial)

`func_ov002_020528cc(u8 *a0, s32 a1, s32 a2)` (0xa8) pulls `[a0+8]` toward a target:
it returns early when `[a0+8] < 0`, when `func_02034148(*(u32 *)(g + <off> + 4))`
exceeds `0x800000`, or when the computed position does not exceed `a1`. Otherwise it
builds `n = ((u16)(<clamp> - a2)) << 1`, calls `func_0203b0c0(([a0+8] >> 8)^2, n)`,
adds the result shifted left 16 to `[g + <off> + 8]`, and finally stores
`func_0204f6ac(((max(a1 - [g+off+8], 0)) >> 8) * (((<clamp> - a2) >> 8) << 1)) << 8`
back into `[a0+8]`. The `func_0203b0c0` with a squared argument is the same routine
`0x02082220` calls, so it is a lookup/roots helper rather than a pure divider.

`func_ov002_020780f8(void)` (0xbc) is the "effect trigger": it ORs bit 1 of a state
word (`orr #2`), clears three bytes at `+0x58`/`+0x59`/`+0x5a` (writing 0, 2 and 0),
then switches on the word at `+0xcc` with cases clustered around `0x14`..`0x18`.

Those two plus `0x02053468` (0xdc), `0x020532e4` (0x184), `0x02057068` (0xa34) and
`func_0203b0c0` (0x20c) are what remain before the family body can be written.

## Full Ghidra pass (2026-11-23)

The headless decompiler was run over the representative and every unresolved
callee in one job (`build/ghidra/dump_f38c.bat`). Signatures recovered:

| callee | signature | notes |
|---|---|---|
| `func_ov002_0205bf64` | `s16 (void)` | `*(s16 *)(g + 0x1e5c)` |
| `func_ov002_02053468` | `s16 (void)` | takes no arguments; the `mov r0,#0x200000` before the call is the `v28` initialiser being scheduled, not an argument |
| `func_ov002_0205bee4` | `s32 (s32, u8 *)` | |
| `func_ov002_0204f770` | `void (s32 amp, u32 angle, s32 *out)` | `out[0..2]`, third word zeroed |
| `func_ov002_020532e4` | `s32 (s32, s32, s32 *, s32)` | |
| `func_0203b0c0` | `s32 (s32)` | one argument at this call site |
| `func_ov002_020528cc` | `void (s32 *, s32, s32)` | |
| `func_ov002_02057068` | `void` x11 args | 3 in registers, 8 on the stack |
| `func_ov002_020780f8` | `void (void)` | |

Algorithm confirmed in order: `n = rec[2] >> 1` (unsigned shift - cast to `u32`
first or mwcc emits `asr`), `sign = 1 - 2n` as an `s8`, `q = rec->0x128 * 4 / 5`,
`x = (func_0205bf64() - 0x14) << 16`, `y = (s16)func_02053468()`,
`m = func_0205bee4(0x14, rec)`, direction branch (`rec->0x4c & 4` selects the
`p38[8]` byte path, else the `rec->0x50` bitfield path; `2` adds, `0` subtracts),
`func_0204f770(q * sign, (s8)(sign * ((y << 8) / 360)) & 0xff, out)`, then
`if (n == 1) v28 = -0x200000`, the two `out[0]`-driven `x` negations,
`y = func_020532e4(x, v28, out, q * sign)`,
`out[2] = func_0203b0c0(0x30000 - sb[8] + y*y*0x157c/2)`,
`func_020528cc(out, 0x320000, 0)`, the 11-argument call, the effect trigger and
`*(u32 *)(g + 0x38) &= ~1`.

`build/cand/g_8f38c.c` holds a complete draft: 584 B against retail's 608 B with
every call, constant and branch in the right order. What is left is allocation,
not algorithm: retail spills `idx` to `sp+0x20` immediately after `mul` and keeps
`rec` in `sl`, with locals laid out `idx 0x20, p38 0x24, v28 0x28, out 0x2c`
(frame `0x3c`); mwcc keeps `idx` in `sl`, `rec` in `r8` and a smaller frame
(`0x34`). Tried and no help: parenthesising the base (`(g + 0x2e4) + i * 0x158`),
a separate `u32 i = idx;`, signed/unsigned loop types, and reordering the
`func_0205bf64`/`q` statements. The remaining lever is a source shape that makes
`idx` not worth a callee-saved register (retail also reloads it for the last
argument), which is the same class of wall as `func_02033f40` in
`notes/ghidra-headless.md`.

