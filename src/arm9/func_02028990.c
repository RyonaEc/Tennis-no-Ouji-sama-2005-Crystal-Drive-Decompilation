#include "types.h"

s32 func_02028990(u8 *a0, u8 *a1, s32 n)
{
    s32 i;

    for (i = 0; i < n; i++)
    {
        u32 x = a0[i] - 0x41;
        u32 y = a1[i] - 0x41;

        if (x <= 0x19) x += 0x20;
        if (y <= 0x19) y += 0x20;
        if (x != y) return x - y;
    }
    return 0;
}