#include "types.h"

u32 func_020217b4(void)
{
    u32 dispcnt;
    u32 addr;
    addr = (*(volatile u16 *)0x04000008 & 0x3c) >> 2 << 14;
    dispcnt = *(volatile u32 *)0x04000000;
    addr = (((dispcnt & 0x07000000) >> 24) << 16) + 0x06000000 + addr;
    return addr;
}