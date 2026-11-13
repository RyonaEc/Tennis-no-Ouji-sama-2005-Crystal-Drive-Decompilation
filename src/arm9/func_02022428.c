#include "types.h"

u32 func_02022428(u32 *a0)
{
    volatile u32 *s = (volatile u32 *)0x04000600;
    if (*s & 0x4000) return (u32)-1;
    *a0 = (*s & 0x1f00) >> 8;
    return 0;
}
