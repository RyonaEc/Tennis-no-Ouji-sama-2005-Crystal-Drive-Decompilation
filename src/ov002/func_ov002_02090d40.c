#include "types.h"

extern u32 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];

extern void func_ov002_02057c4c(u32 a0, u32 a1, u32 a2, u8 a3, s32 s0);
extern void func_ov002_02057a9c(u32 a0, u32 a1, u32 a2, u8 a3,
                                u32 s0, u32 s1, u32 s2, u32 s3);

void func_ov002_02090d40(s32 a0)
{
    u32 base = data_ov002_02102070[0];
    u8 *q = (u8 *)(base + 0x2e4 + a0 * 0x158);
    u32 id = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    u32 lane = (id >> 1) & 0xff;
    u32 f = (lane == 0) ? 1 : 0;
    u8 *tbl = (u8 *)(data_ov002_020fb544 + *(u8 *)q * 0x34);

    func_ov002_02057c4c(*(u32 *)(q + 4), *(u32 *)(q + 8), *(u32 *)(tbl + 8), f, a0);
    func_ov002_02057a9c(0, 0, 0x20000, f, 0x10000000, *(u8 *)q, 0, a0);
}