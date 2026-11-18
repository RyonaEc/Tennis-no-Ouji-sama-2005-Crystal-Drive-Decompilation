#include "types.h"

typedef struct { u32 lo, hi, pad, sign; } Q;

u32 func_02036824(Q q)
{
    volatile u32 *hi = &q.hi;

    *hi = (q.hi & 0x7fffffff) | (q.sign & 0x80000000);
    return q.lo;
}