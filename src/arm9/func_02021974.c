#include "types.h"

u32 func_02021974(void)
{
    u32 mode = *(volatile u32 *)0x04000000 & 7;
    u16 d = *(volatile u16 *)0x0400000c;
    u32 x = (*(volatile u32 *)0x04000000 & 0x38000000) >> 27;
    u32 addr = x << 16;
    u32 y = ((u32)d & 0x1f00) >> 8;

    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return addr + 0x06000000 + (y << 11);
    case 5:
        if (d & 0x80)
            return (y << 14) + 0x06000000;
        return addr + 0x06000000 + (y << 11);
    case 6:
        return 0x06000000;
    }
    return 0;
}