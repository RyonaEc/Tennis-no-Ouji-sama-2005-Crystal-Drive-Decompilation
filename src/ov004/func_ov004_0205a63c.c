#include "types.h"

void func_ov004_0205a63c(void)
{
    u8 *g = *(u8 **)0x020690a0;
    u8 *p = g + 0x13a8;
    u32 v;

    if (*(u32 *)(p + 0x1c) == 0)
        return;
    v = *(u32 *)(p + 0x2c);
    *(u16 *)0x04001008 = (*(u16 *)0x04001008 & 0x43) | 0x80 | (v << 8) | (v << 2);
    *(u32 *)(p + 0x24) = 0;
    *(u32 *)(p + 0x20) = 1;
}