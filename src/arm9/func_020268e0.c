#include "types.h"

extern void func_01ff84b4(u32 a0, u32 a1, u32 a2, u32 a3);

void func_020268e0(u32 a0, u32 a1, u32 a2, u32 a3)
{
    volatile u32 *dma = (volatile u32 *)0x40000b0;
    volatile u32 *p;

    if (a3 == 0)
        return;
    p = dma + (a0 * 3 + 2);
    while (*p & 0x80000000)
        ;
    func_01ff84b4(a0, a1, a2, (a3 >> 2) | 0x84000000);
    while (*p & 0x80000000)
        ;
}