#include "types.h"

u32 func_ov002_0204f6ac(u32 a0)
{
    volatile u16 *ie = (volatile u16 *)0x04000208;
    volatile u16 *dma = (volatile u16 *)0x040002b0;
    u16 old = *ie;

    *ie = 0;
    *dma = 0;
    *(volatile u32 *)0x040002b8 = a0;
    (void)*ie;
    *ie = old;
    while (*dma & 0x8000)
        ;
    return *(volatile u16 *)0x040002b4;
}
