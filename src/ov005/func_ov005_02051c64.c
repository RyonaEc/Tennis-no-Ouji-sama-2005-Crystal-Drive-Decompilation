#include "types.h"

u32 func_ov005_02051c64(u32 a0, u32 *a1)
{
    u32 s = 0x2000;
    u16 i;
    s32 n;

    if (a0 < 0x80) {
        n = a0 + 1;
        for (i = 0; i < n; i++) {
            if (i != 0)
                s += a1[i - 1];
        }
    }
    return s;
}