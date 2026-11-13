#include "types.h"

u32 func_020223fc(u32 *a0)
{
    volatile u32 *s = (volatile u32 *)0x04000600;
    if (*s & 0x4000) return (u32)-1;
    *a0 = (*s & 0x2000) >> 0xd;
    return 0;
}
