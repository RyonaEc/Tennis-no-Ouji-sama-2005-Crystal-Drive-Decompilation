#include "types.h"

void func_020182e8(u8 *o)
{
    u8 *p;
    u8 *q = o + 0x24;

    p = *(u8 **)(q + 8);
    while (p)
    {
        *(u32 *)(p + 8) = *(u32 *)(o + 0x1c);
        p = *(u8 **)(p + 0xc);
    }
    *(u32 *)(q + 4) = *(u32 *)(o + 0x1c);
}