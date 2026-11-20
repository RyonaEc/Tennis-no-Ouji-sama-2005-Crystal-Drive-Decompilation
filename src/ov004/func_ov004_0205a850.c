#include "types.h"

extern s32 func_ov004_0204dcb4(u32, u32);
extern s32 func_ov004_0204dce4(u32, s32);

s32 func_ov004_0205a850(u32 a0)
{
    u8 *g = *(u8 **)0x020690a0;
    u8 *p = g + 0x13a8;

    if (*(u32 *)*(u32 *)(p + 4) != 0)
        return 0;
    if (*(u32 *)(p + 0x14) != 0)
        *(u32 *)(p + 0x14) = 0;
    *(u32 *)(p + 0xc) = 0;
    if (func_ov004_0204dcb4(*(u32 *)(p + 4), a0) == 0)
        return 0;
    return func_ov004_0204dce4(*(u32 *)(p + 4), 2);
}