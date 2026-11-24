#include "types.h"

void func_02022554(u32 enable, u32 fogMode, u32 fogSlope, s32 fogOffset)
{
    if (enable) {
        *(volatile u16 *)0x0400035c = (u16)fogOffset;
        *(volatile u16 *)0x04000060 = (u16)((*(volatile u16 *)0x04000060 & ~0x3f40) |
                                            ((fogSlope << 8) | (fogMode << 6) | 0x80));
    } else {
        *(volatile u16 *)0x04000060 &= (u16)~0x3080;
    }
}