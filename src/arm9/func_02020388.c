#include "types.h"

u32 func_02020388(s32 a0)
{
    volatile u16 *p = (volatile u16 *)0x04000004;
    u32 r = *p & 0x10;
    if (a0 != 0) *p |= 0x10;
    else *p &= ~0x10;
    return r;
}
