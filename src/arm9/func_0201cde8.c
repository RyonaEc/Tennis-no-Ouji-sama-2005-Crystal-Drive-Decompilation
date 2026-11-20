#include "types.h"

extern u32 func_0201bef4(u32);

u32 func_0201cde8(u8 *a0, s32 a1, u8 *a2)
{
    u32 m = 0;
    s32 i;

    for (i = 0; i < a1; i++)
    {
        (a0 + i)[0x4c] = a2[i];
        m |= 1 << a2[i];
    }
    if (func_0201bef4(m) == 0) return 0;
    *(u32 *)(a0 + 0x48) = a1;
    *(u32 *)(a0 + 0x44) = m;
    return 1;
}