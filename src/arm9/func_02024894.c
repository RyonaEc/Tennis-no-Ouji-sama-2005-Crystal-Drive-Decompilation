#include "types.h"

u32 func_02024894(u32 a0)
{
    u16 old = *(volatile u16 *)0x04000208;
    u32 v;
    u16 dummy;
    *(volatile u16 *)0x04000208 = 0;
    v = *(volatile u32 *)0x04000214;
    *(volatile u32 *)0x04000214 = a0;
    dummy = *(volatile u16 *)0x04000208;
    *(volatile u16 *)0x04000208 = old;
    return v;
}
