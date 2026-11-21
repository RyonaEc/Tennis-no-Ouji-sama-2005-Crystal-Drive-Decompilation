#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

s32 func_ov002_020633e0(u32 a0)
{
    u8 *p = B + 0x1744 + a0 * 0x38;
    u32 *q;

    if ((*(u32 *)(p + 0x24) & 0x2000000) == 0)
        return 0;
    q = (u32 *)(p + 0x24);
    *q |= 0x1000000;
    *q &= ~0x2000000;
    return 1;
}