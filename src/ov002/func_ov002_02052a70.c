#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02052a70(u32 idx)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *e = base + 0x2e4 + idx * 0x158;
    u8 *g = base + 0x1824;
    s32 v;

    v = *(s32 *)(e + 4);
    if (v > 0) {
        *(s32 *)(g + 0x94) = -0x680000;
        *(s32 *)(g + 0x98) = 0;
    } else {
        *(s32 *)(g + 0x94) = 0;
        *(s32 *)(g + 0x98) = 0x680000;
    }
    v = *(s32 *)(e + 8);
    if (v > 0) {
        *(s32 *)(g + 0x9c) = -0x430000;
        *(s32 *)(g + 0xa0) = 0;
    } else {
        *(s32 *)(g + 0x9c) = 0;
        *(s32 *)(g + 0xa0) = 0x430000;
    }
}
