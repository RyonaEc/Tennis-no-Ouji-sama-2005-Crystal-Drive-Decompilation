#include "types.h"

extern u8 data_ov008_0204f440[];

s32 func_ov008_0204d970(void)
{
    u8 *g = *(u8 **)data_ov008_0204f440;
    u32 m;
    s32 i;
    s32 n;

    n = g[0xb];
    m = *(u32 *)(g + 0x3c);
    for (i = 0; i < n; i++) {
        if (m & (1 << i)) {
            u8 *p = g + i * 2;

            p[0x10] = 1;
            g[0x21] = p[0xf];
            return 1;
        }
    }
    return 0;
}
