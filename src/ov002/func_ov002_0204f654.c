#include "types.h"

u32 func_ov002_0204f654(u32 a0, u32 a1)
{
    volatile u32 *p = (volatile u32 *)0x04000290;
    volatile u32 *q = (volatile u32 *)0x04000298;

    *(volatile u16 *)0x04000280 = 0;
    p[0] = a0;
    q[0] = a1;
    q[1] = 0;
    while (*(volatile u16 *)0x04000280 & 0x8000) {
    }
    return *(volatile u32 *)0x040002a0;
}