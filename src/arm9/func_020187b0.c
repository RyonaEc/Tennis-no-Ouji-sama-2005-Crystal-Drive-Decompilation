#include "types.h"

extern s32 func_02018b90(u8 *a0);
extern u32 func_02018bbc(u8 *a0, u32 a1);

s32 func_020187b0(u8 *a0)
{
    if (func_02018b90(a0) != 0)
        return 0;
    *(u16 *)(a0 + 8) = func_02018bbc(a0, *(u16 *)(a0 + 8));
    *(u16 *)(a0 + 0xc) -= 1;
    return 1;
}