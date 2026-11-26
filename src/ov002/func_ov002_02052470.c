#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_02089a6c(u32 a0);

u32 func_ov002_02052470(s32 a0, s32 a1)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u32 t = ((u32)*(u8 *)(base + 0x158 * a0 + 0x2e6) >> 1) & 0xff;
    s32 sv = *(s16 *)(base + 0x3656 + t * 4);
    s32 q = (s16)(sv / 20 - 2);
    u16 out;
    s32 m;
    u32 a;

    if (q == 3)
        q = 2;
    m = q + 4;
    out = (u16)(a1 * m / 4);
    a = func_ov002_02089a6c(t);
    if (a != 8) {
        if (a == 9)
            out = (u16)(out << 1);
    } else {
        out = (u16)(3 * out / 2);
    }
    return out;
}