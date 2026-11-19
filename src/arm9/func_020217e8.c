#include "types.h"

u32 func_020217e8(void)
{
    u32 t = *(volatile u32 *)0x04001000 & 7;
    u16 r = *(volatile u16 *)0x0400100e;
    u32 m = r & 0x1f00;
    u32 base = m >> 8;

    switch (t) {
    case 0:
    case 1:
    case 2:
        return 0x06200000 + (base << 11);
    case 3:
    case 4:
    case 5:
        if (r & 0x80)
            return 0x06200000 + (base << 14);
        return 0x06200000 + (base << 11);
    case 6:
        return 0;
    }
    return 0;
}