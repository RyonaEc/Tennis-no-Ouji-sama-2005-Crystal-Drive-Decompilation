#include "types.h"

u32 func_02021a74(void)
{
    u32 dispcnt;
    u32 addr;
    addr = (*(volatile u16 *)0x04000008 & 0x1f00) >> 8 << 11;
    dispcnt = *(volatile u32 *)0x04000000;
    addr = (((dispcnt & 0x38000000) >> 27) << 16) + 0x06000000 + addr;
    return addr;
}