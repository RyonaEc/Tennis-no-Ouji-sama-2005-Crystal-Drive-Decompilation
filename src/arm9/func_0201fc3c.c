#include "types.h"

extern u32 func_0203b0c0(u32 a0, u32 a1);

u32 func_0201fc3c(u32 *p)
{
    u32 lo;
    if ((s32)p[2] >= (s32)p[3])
        return p[1];
    lo = p[0];
    return lo + func_0203b0c0((s32)p[2] * (p[1] - lo), (s32)p[3]);
}