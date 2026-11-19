#include "types.h"

extern u32 data_ov005_020c9740;

u32 func_ov005_0204f3cc(void)
{
    u8 *g = (u8 *)data_ov005_020c9740;
    u32 s2;
    u32 idx;

    *(u32 *)(g + 0xc) = *(u32 *)(g + 0xc) * 0x41c64e6d + 0x3039;
    s2 = *(u32 *)(g + 0xc);
    idx = ((*(u16 *)(g + 0x10) & 0xff) + 1) & 3;
    *(u16 *)(g + 0x10) = (u8)idx;
    return (u16)((s2 >> 16) >> ((*(u16 *)(g + 0x10)) + 3));
}