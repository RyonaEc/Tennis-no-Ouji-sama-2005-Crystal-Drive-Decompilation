#include "types.h"

void func_0202250c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    u32 v = a0 | (a1 << 16);
    v = v | (a3 << 24);
    if (a4 != 0) v |= 0x8000;
    *(volatile u32 *)0x04000350 = v;
    *(volatile u16 *)0x04000354 = a2;
}
