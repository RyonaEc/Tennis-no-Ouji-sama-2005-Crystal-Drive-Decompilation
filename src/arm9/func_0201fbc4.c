#include "types.h"

u8 *func_0201fbc4(u8 *a0, s32 i)
{
    u32 n;

    if (i < 0)
        return 0;
    n = *(u32 *)(a0 + 0x1c);
    if ((u32)i >= n)
        return 0;
    if (*(u32 *)(a0 + 0x20 + i * 0xc) == 0xffffffff)
        return 0;
    return a0 + 0x20 + i * 0xc;
}