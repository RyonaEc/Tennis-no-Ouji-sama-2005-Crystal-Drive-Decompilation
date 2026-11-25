#include "types.h"

extern u32 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];
extern void func_ov002_02057c4c(u32 a0, u32 a1, u32 a2, u8 a3, s32 s0);
extern void func_ov002_02057a9c(u32 a0, u32 a1, u32 a2, u8 a3, u32 s0, u32 s1, u32 s2, u32 s3);

void func_ov002_020a5d48(s32 a0)
{
    u32 base = data_ov002_02102070[0];
    s32 off = a0 * 0x158;
    u8 *e = (u8 *)(base + 0x2e4) + off;
    u32 flag = (((u32)*(u8 *)(base + off + 0x2e6) >> 1) & 0xff) == 0;

    func_ov002_02057c4c(*(u32 *)(e + 4), *(u32 *)(e + 8),
                        *(u32 *)(data_ov002_020fb544 + e[0] * 0x34 + 8),
                        (u8)flag, a0);
    func_ov002_02057a9c(0, 0, 0x1e666, (u8)flag, 0x10000000, e[0], 0, a0);
}