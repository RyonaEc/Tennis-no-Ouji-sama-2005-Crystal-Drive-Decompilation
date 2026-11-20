#include "types.h"

extern u32 func_0202a3a0(u8 *a0, u32 a1, u32 a2);
extern u32 func_0202a060(u8 *a0);

s32 func_02029fb0(u32 a0, u32 a1)
{
    u8 buf[0x24];
    if (func_0202a3a0(buf, a0, a1) != 0)
    {
        if (func_0202a060(buf) != 0)
            goto one;
    }
    goto zero;
zero:
    return 0;
one:
    return 1;
}