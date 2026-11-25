#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
extern u32 func_ov002_020855b8(u32 a0);

s32 func_ov002_020c262c(s32 a0)
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
    if ((*(u32 *)(e + 0xd8) & 1) != 0) {
        *(u16 *)(e + 0x66) = 0;
        func_ov002_0204d270(v, *(u8 *)(e + 1), 0x2d);
        *(u8 *)(e + 0xd1) = 1;
        ret = 1;
        *(u8 *)(base + a0 + 0x142) = 0;
        func_ov002_020855b8(a0);
    }
    return ret;
}