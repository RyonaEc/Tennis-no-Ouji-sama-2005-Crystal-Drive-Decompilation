#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_020855dc(u8 *a0, u8 *a1);

void func_ov002_020856a0(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *fixed = base + 0x2e4 + a0 * 0x158;
    u8 *e = base + 0x2e4;
    s32 i;
    u8 *q = base + 0x186;
    u8 *p = base + 0x156 + a0 * 0xc;

    for (i = 0; i < 4; i++) {
        if (e[0] != 0) {
            if (((u32)e[2] >> 1) != ((u32)fixed[2] >> 1))
                func_ov002_020855dc(p, q);
        }
        q += 0xc;
        e += 0x158;
    }
}