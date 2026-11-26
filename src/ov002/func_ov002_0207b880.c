#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_02034148(s32 a0);
extern u32 func_ov002_0204f70c(s32 a0, s32 a1);
extern s32 func_ov002_0207b864(s32 a0);

u32 func_ov002_0207b880(s16 *a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *s = base + 0x1824;
    u8 *t = base + 0x2e4;
    u8 *e;
    s16 x, y, v;

    if (!(*(u32 *)(s + 0xc8) & 8))
        return 0;
    e = (u8 *)(0x158 * *(u8 *)(s + 0x8f) + t);
    x = (s16)(func_02034148(*(u32 *)(s + 4) - *(u32 *)(e + 8)) >> 16);
    {
        s32 dy = *(u32 *)s - *(u32 *)(e + 4);

        y = (s16)(-dy >> 16);
    }
    v = (s16)func_ov002_0204f70c(x, y);
    *a0 = (s16)func_ov002_0207b864((s16)((v * 0x168) / 0x10000));
    return 1;
}