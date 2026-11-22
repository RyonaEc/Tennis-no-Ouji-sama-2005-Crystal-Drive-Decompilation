#include "types.h"

u32 func_ov002_0207ccd4(s32 swap, s32 *p, s32 lo, s32 hi, s32 lo2, s32 hi2)
{
    u32 ok = 1;

    if (swap == 0) {
        s32 t = hi2;
        hi2 = -hi;
        hi = -t;
    }
    if (p[0] < lo || p[0] > lo2 || p[1] < hi || p[1] > hi2)
        ok = 0;
    return ok;
}