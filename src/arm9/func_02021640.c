#include "types.h"

u32 func_02021640(void)
{
    s32 t = *(volatile u32 *)0x04000000 & 7;
    u16 r = *(volatile u16 *)0x0400000e;

    if (t < 3 || (t < 6 && (r & 0x80) == 0))
        return 0x06000000 + ((*(volatile u32 *)0x04000000 & 0x7000000) >> 24 << 16)
                          + (((u32)r & 0x3c) >> 2 << 14);
    return 0;
}