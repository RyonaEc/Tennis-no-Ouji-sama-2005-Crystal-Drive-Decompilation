#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0206004c(void);

s32 func_ov002_0204f8d8(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *t = base + 0x4bc4;
    u32 x;
    u32 k;

    if ((a0 & 0x4000) != 0)
        return 1;
    if ((a0 & 0x10000) != 0) {
        x = func_ov002_0206004c();
        k = *(u8 *)(t + x);
        if (*(u8 *)((u8 *)data_ov002_02102070[0] + x * 0x158 + 0x3fd) >= 4)
            return 0;
        k = *(u8 *)(base + k + 0xc5);
        if (k == 0xa || k == 0x12)
            return 0;
        return 1;
    }
    return 0;
}