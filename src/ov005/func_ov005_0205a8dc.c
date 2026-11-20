#include "types.h"

s32 func_ov005_0205a8dc(u32 a0, u32 a1)
{
    u8 *b = *(u8 **)0x20c9740;
    s32 v = *(u16 *)(b + 0x7b70 + (a0 * 10 + a1) * 2) + *(u16 *)(b + 0x7296);

    v = (s16)(v % 0x168);
    if (v < 0x5a)
        return v;
    if (v < 0xb4)
        return (s16)v;
    if (v < 0x10e)
        return (s16)(0x168 - v);
    return (s16)(0x5a - (v - 0x10e));
}