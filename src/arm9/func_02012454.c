#include "types.h"

extern u8 *func_0200faa8(s32);

void func_02012454(s32 a0, s32 a1)
{
    u8 *p = func_0200faa8(a0);
    s32 idx;
    s32 bit;

    if (p == 0) return;
    idx = a0 / 8;
    bit = a0 % 8;
    if (a1 != 0) p[idx] |= (u8)(1 << bit);
    else p[idx] &= (u8)~(1 << bit);
}