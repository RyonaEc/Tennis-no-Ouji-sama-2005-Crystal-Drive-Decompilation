#include "types.h"

extern u32 func_0202a3a0(u8 *a0, u32 a1, u32 a2);
extern u32 func_0202a318(u8 *a0);
extern void func_0202a19c(u8 *a0);

s32 func_0202a004(u32 a0, u32 a1)
{
    u8 buf[0x24];
    if (func_0202a3a0(buf, a0, a1) == 0)
        goto zero;
    if (func_0202a318(buf) != 0)
        goto one;
zero:
    return 0;
one:
    func_0202a19c(buf);
    return 1;
}