#include "types.h"

void func_02030660(u32 *out)
{
    volatile u16 *reg = (volatile u16 *)0x04000204;

    out[0] = (*reg & 0xc) >> 2;
    out[1] = (*reg & 0x10) >> 4;
    *reg = (*reg & ~0xc) | 0xc;
    *reg &= ~0x10;
}