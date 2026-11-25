#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d22c(s32 a0, s32 a1, s32 a2);
extern void func_ov002_020a3d88(s32 a0, s32 a1);

s32 func_ov002_020a5320(s32 a0)
{
    s32 off = a0 * 0x158;
    u8 *e;
    u8 *base;
    u8 v;
    s32 ret;

    base = (u8 *)data_ov002_02102070[0];
    e = base + 0x2e4 + off;
    v = *(u8 *)(base + off + 0x2e6);
    ret = 0;
    if ((*(u32 *)(e + 0xd8) & 4) != 0) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x6a) = 0;
        func_ov002_0204d22c(v, *(u8 *)(e + 1), 0xf);
        if ((*(u32 *)(e + 0x4c) & 4) != 0)
            func_ov002_020a3d88(a0, 0);
        *(u8 *)(e + 0xd1) = 3;
        ret = 1;
    }
    return ret;
}