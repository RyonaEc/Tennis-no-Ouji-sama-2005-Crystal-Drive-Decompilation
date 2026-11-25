#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_02034148(s32 a0);
extern u32 func_0203b0c0(u32 a0, u32 a1);
extern u32 func_ov002_0204f6ac(u32 a0);

void func_ov002_020528cc(u8 *obj, s32 a1, s32 a2)
{
    u32 base = data_ov002_02102070[0];
    u8 *p = (u8 *)(base + 0x1824);
    s32 v = *(s32 *)(obj + 8);
    s32 d;
    s32 t;

    if (v < 0)
        return;
    if (func_02034148(*(s32 *)(p + 4)) > 0x800000)
        return;
    d = 0x157c - a2;
    t = *(s32 *)(p + 8);
    if (t + (s32)(func_0203b0c0((v >> 8) * (v >> 8), d << 1) << 16) <= a1)
        return;
    v = a1 - t;
    if (v < 0)
        v = 0;
    *(s32 *)(obj + 8) = func_ov002_0204f6ac((v >> 8) * ((d >> 8) << 1)) << 8;
}