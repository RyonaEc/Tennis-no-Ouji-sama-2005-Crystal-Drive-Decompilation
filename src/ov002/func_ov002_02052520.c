#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_02034148(s32 a0);

void func_ov002_02052520(s32 x, s32 y, s32 z)
{
    u32 base = data_ov002_02102070[0];
    u8 *p = (u8 *)(base + 0x1824);
    s32 ax = func_02034148(x);
    s32 ay = func_02034148(y);
    s32 dx, dy, dz;

    if (z <= 0x30000)
        z = 0x40000;
    if (ax > 0x12c0000) {
        if (x < 0)
            x = -0x12c0000;
        else
            x = 0x12c0000;
    }
    if (ay < 0x10000) {
        if (y < 0)
            y = -0x10000;
        else
            y = 0x10000;
    } else if (ay > 0xcb0000) {
        if (y < 0)
            y = -0xcb0000;
        else
            y = 0xcb0000;
    }
    dx = x - *(s32 *)(p + 0);
    dy = y - *(s32 *)(p + 4);
    dz = z - *(s32 *)(p + 8);
    *(s32 *)(p + 0xc) += dx;
    *(s32 *)(p + 0x10) += dy;
    *(s32 *)(p + 0x14) += dz;
    *(s32 *)(p + 0) = x;
    *(s32 *)(p + 4) = y;
    *(s32 *)(p + 8) = z;
}