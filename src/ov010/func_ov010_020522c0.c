#include "types.h"

s32 func_ov010_020522c0(u16 *a0, u16 *a1)
{
    s32 i;
    s32 n;

    n = a0[0];
    if (n != a1[0])
        return 0;
    if (n == 0)
        return 1;
    for (i = 0; i < n; i++) {
        u16 *p = a0 + i;
        u16 *q = a1 + i;

        if (p[1] != q[1])
            return 0;
    }
    return 1;
}