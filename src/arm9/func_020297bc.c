#include "types.h"

u32 func_020297bc(u8 *a0, u32 a1, u32 a2)
{
    u32 lo;
    u32 hi;

    switch (a2)
    {
    case 0:
        a1 += *(u32 *)(a0 + 0x20);
        break;
    case 1:
        a1 += *(u32 *)(a0 + 0x28);
        break;
    case 2:
        a1 += *(u32 *)(a0 + 0x24);
        break;
    default:
        return 0;
    }
    lo = *(u32 *)(a0 + 0x20);
    if (a1 < lo) a1 = lo;
    hi = *(u32 *)(a0 + 0x24);
    if ((s32)a1 > (s32)hi) a1 = hi;
    *(u32 *)(a0 + 0x28) = a1;
    return 1;
}