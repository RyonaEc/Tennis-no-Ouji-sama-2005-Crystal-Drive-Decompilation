#include "types.h"

extern u8 *data_ov003_020eb720;

s32 func_ov003_02081ecc(u32 a0, u32 a1)
{
    u8 *b = data_ov003_020eb720;
    s32 v = *(u16 *)(b + 0xbb68 + (a0 * 9 + a1) * 2) + *(u16 *)(b + 0xb626);

    v = (s16)(v % 0x168);
    if (v < 0x5a)
        return v;
    if (v < 0xb4)
        return (s16)v;
    if (v < 0x10e)
        return (s16)(0x168 - v);
    return (s16)(0x5a - (v - 0x10e));
}