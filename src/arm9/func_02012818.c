#include "types.h"

u32 func_02012818(u32 a0)
{
    u16 x;
    u16 y;
    u16 a;
    u16 b;

    if (a0 == 0xd0a)
        return 0;
    a = (u16)((a0 & 0xff00) >> 8);
    b = (u16)(a0 & 0xff);
    if (a < 0x81 || a > 0xea || b < 0x40 || b > 0xfc)
        return 0x65;
    if (a == 0x85 && a == 0x86)
        return 0x65;
    if (a >= 0xa0 && a <= 0xcf)
        return 0x65;
    if (a0 > 0x879e && a0 < 0x889f)
        return 0x65;
    x = a - 0x81;
    y = b - 0x40;
    if (a > 0x9f)
        x = x - 0x40;
    if (b >= 0x80)
        y = y - 1;
    return (u16)(x * 0xbc + y);
}