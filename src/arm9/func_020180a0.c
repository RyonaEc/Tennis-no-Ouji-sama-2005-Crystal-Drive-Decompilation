#include "types.h"

u8 *func_020180a0(u32 *p, u16 v)
{
    u8 *q = (u8 *)*p;

    *(volatile u16 *)q = v;
    *(volatile u16 *)(q + 2) = 0;
    *(volatile u32 *)(q + 4) = p[1] - ((u32)q + 0x10);
    *(volatile u32 *)(q + 8) = 0;
    *(volatile u32 *)(q + 0xc) = 0;
    return q;
}
