#include "types.h"

u32 func_020216a0(void)
{
    s32 t = *(volatile u32 *)0x04001000 & 7;
    u16 r = *(volatile u16 *)0x0400100c;

    if (t < 5 || (r & 0x80) == 0)
        return 0x06200000 + (((u32)r & 0x3c) >> 2 << 14);
    return 0;
}