#include "types.h"

extern s32 func_020291f4(u8 *a0, u32 a1);

s32 func_020298e4(u8 *a0)
{
    if (func_020291f4(a0, 8) == 0)
        return 0;
    *(u32 *)(a0 + 8) = 0;
    *(u32 *)(a0 + 0x10) = 0xe;
    *(u32 *)(a0 + 0xc) &= ~0x30;
    return 1;
}