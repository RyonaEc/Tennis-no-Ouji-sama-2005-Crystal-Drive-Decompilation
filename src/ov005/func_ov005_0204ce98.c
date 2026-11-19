#include "types.h"

s32 func_ov005_0204ce98(u32 a0, u32 a1, u16 *src, u8 *dst)
{
    u16 i;
    u16 j;

    for (j = 0; j < 0x11; j++) {
        u16 *row = (u16 *)(dst + j * 0x20);

        for (i = 0; i < 0x10; i++) {
            u16 v;

            v = j + (src[i] & 0x1f);
            if (v > 0x1f)
                v = 0x1f;
            row[i] = v;
            v = j + ((src[i] & 0x3e0) >> 5);
            if (v > 0x1f)
                v = 0x1f;
            row[i] += v << 5;
            v = j + ((src[i] & 0x7c00) >> 10);
            if (v > 0x1f)
                v = 0x1f;
            row[i] += v << 10;
        }
    }
    return 0;
}