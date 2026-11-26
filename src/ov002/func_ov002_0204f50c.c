#include "types.h"

extern u32 data_ov002_02102070;

u32 func_ov002_0204f50c(u32 a0, u32 a1, u32 a2)
{
    u8 *base = (u8 *)data_ov002_02102070;
    u8 *p = base + 0x1a60a8;
    volatile u8 *cnt = base + 0x36d8;
    u32 n;
    u8 *slot;

    n = *(u32 *)(p + *cnt * 4 + 0x608);
    slot = p + 0x610 + *cnt * 0x300;
    if (n >= 0x40)
        return 0;
    *(u32 *)(slot + n * 0xc) = a0;
    *(u32 *)(slot + n * 0xc + 4) = a1;
    *(u32 *)(slot + n * 0xc + 8) = a2;
    *(u32 *)(p + 0x608 + *cnt * 4) += 1;
    return 1;
}