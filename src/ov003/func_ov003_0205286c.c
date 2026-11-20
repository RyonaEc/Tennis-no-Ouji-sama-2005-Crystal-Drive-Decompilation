#include "types.h"

s32 func_ov003_0205286c(u32 a0, u32 a1, u16 *a2, u16 *a3)
{
    u16 j, i;

    for (i = 0; i < 0x11; i++) {
        u16 *d = a3 + i * 16;

        for (j = 0; j < 0x10; j++) {
            u16 v;

            v = (u16)((a2[j] & 0x1f) + i * 2);
            if (v > 0x1f)
                v = 0x1f;
            d[j] = v;
            v = (u16)(((a2[j] & 0x3e0) >> 5) + i * 2);
            if (v > 0x1f)
                v = 0x1f;
            d[j] += v << 5;
            v = (u16)(((a2[j] & 0x7c00) >> 10) + i * 2);
            if (v > 0x1f)
                v = 0x1f;
            d[j] += v << 10;
        }
    }
    return 0;
}