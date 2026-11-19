#include "types.h"

void func_0203062c(u32 *p)
{
    volatile u16 *reg = (volatile u16 *)0x04000204;

    *reg = (p[0] << 2) | (*reg & ~0xc);
    *reg = (p[1] << 4) | (*reg & ~0x10);
}