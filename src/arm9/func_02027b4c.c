#include "types.h"

extern u8 data_02041b78[];

u32 func_02027b4c(s32 a0)
{
    u32 v;

    if (a0 < -0x2d3)
        a0 = -0x2d3;
    else if (a0 > 0)
        a0 = 0;
    v = *(u8 *)(data_02041b78 + (a0 + 0x2d3));
    if (a0 < -0xf0)
        a0 = 3;
    else if (a0 < -0x78)
        a0 = 2;
    else if (a0 < -0x3c)
        a0 = 1;
    else
        a0 = 0;
    return (u16)(v | (a0 << 8));
}