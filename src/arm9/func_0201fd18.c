#include "types.h"

void func_0201fd18(u32 a0, u32 a1)
{
    volatile u32 *p = (volatile u32 *)0x04000290;
    volatile u32 *q = (volatile u32 *)0x04000298;

    *(volatile u16 *)0x04000280 = 1;
    p[0] = 0;
    p[1] = a0;
    q[0] = a1;
    q[1] = 0;
}