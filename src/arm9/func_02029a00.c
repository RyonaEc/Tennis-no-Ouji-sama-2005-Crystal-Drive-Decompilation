#include "types.h"

extern s32 func_020291f4(u8 *a0, u32 a1);

s32 func_02029a00(u8 *a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u32 *p;

    *(u32 *)(a0 + 8) = a1;
    *(u32 *)(a0 + 0x34) = a4;
    *(u32 *)(a0 + 0x2c) = a2;
    *(u32 *)(a0 + 0x30) = a3;
    if (func_020291f4(a0, 7) == 0)
        return 0;
    p = (u32 *)(a0 + 0xc);
    *p = *p | 0x10;
    *p = *p & ~0x20;
    return 1;
}