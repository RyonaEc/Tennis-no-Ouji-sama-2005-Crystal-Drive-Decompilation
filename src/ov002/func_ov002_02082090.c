#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02082090(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    u8 *r = g + 0x2e4;
    u8 *t = g + 0x36ac;
    u32 n;

    if (r[0] == 0)
        return;
    n = (u32)r[2] >> 1;
    if (n == 0) {
        if (g[0x100] == 3)
            goto set;
    }
    if (n != 1)
        return;
    if (g[0x100] != 2)
        return;
set:
    t[0x2a] = 1;
}