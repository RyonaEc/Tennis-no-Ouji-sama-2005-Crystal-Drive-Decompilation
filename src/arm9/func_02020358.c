#include "types.h"

u32 func_02020358(s32 a0)
{
    volatile u16 *p = (volatile u16 *)0x04000004;
    u32 r = *p & 8;
    if (a0 != 0) *p |= 8;
    else *p &= ~8;
    return r;
}
