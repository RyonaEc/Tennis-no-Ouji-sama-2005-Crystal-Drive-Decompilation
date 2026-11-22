#include "types.h"

extern s32 func_0203b0c0(s32 a0, s32 a1);

s32 func_ov002_0204d4d8(s32 a0, s32 a1)
{
    s32 n, lo, hi, v;

    if (a0 < 0x81) {
        n = 0x81;
        lo = 0xa00;
        hi = 0xe00;
    } else {
        n = 0x82;
        a0 -= 0x81;
        lo = 0x200;
        hi = 0x600;
    }
    v = lo + func_0203b0c0((n - 1 - a0) * (hi - lo), n) + a1;
    if (v > hi)
        return hi;
    if (v < lo)
        v = lo;
    return v;
}