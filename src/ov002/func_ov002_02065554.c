#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

u32 func_ov002_02065554(u32 a0, s16 *a1)
{
    u8 *e;
    u32 r = 0;
    s32 near = 0;
    s32 dx, dy;
    u8 st;

    st = *(u8 *)(B + a0 * 0x38 + 0x1744);
    e = B + 0x2e4 + a0 * 0x158;
    if (st == 2)
        near = 1;
    if (*(s32 *)(e + 0xf8) <= 124518)
        near = 1;
    if (!near)
        return 0;
    dx = *(s16 *)(e + 0xa) - a1[3];
    dy = *(s16 *)(e + 6) - a1[1];
    if (dy * dy + dx * dx > 0x1000)
        r = 0x300;
    return r;
}