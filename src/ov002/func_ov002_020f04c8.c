#include "types.h"

extern u32 data_ov002_02102070[];

extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);

s32 func_ov002_020f04c8(s32 a0)
{
    u32 base = data_ov002_02102070[0];
    u8 *p = (u8 *)(base + 0x2e4 + a0 * 0x158);
    u8 id = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    s32 r2;
    s32 r = 0;

    if ((*(u32 *)(p + 0xd8) & 4) != 0) {
        *(u16 *)(p + 0x66) = 0;
        *(u16 *)(p + 0x8e) = 0x31;
        *(u16 *)(p + 0x8c) = p[0x80];
        if (p[0x7c] == 0) {
            if (p[2] < 2)
                r2 = 0x2e;
            else
                r2 = 0x2f;
        } else {
            if (p[2] < 2)
                r2 = 0x2f;
            else
                r2 = 0x2e;
        }
        func_ov002_0204d270(id, p[1], r2);
        p[0xd1] = 3;
        r = 1;
    }
    return r;
}