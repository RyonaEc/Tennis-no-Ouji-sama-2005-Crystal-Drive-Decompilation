#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02078208(s32 a0);

s32 func_ov002_020eeb98(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u8 v = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    u8 *f = base + 0x1824;
    s32 d;
    s32 ret = 0;

    if ((*(u32 *)(e + 0xd8) & 8) != 0) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = *(u16 *)(f + 0xac);
        *(u16 *)(e + 0x8c) = *(u16 *)(f + 0xac);
        *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
        *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
        if (*(u8 *)(e + 2) < 2)
            d = 0x100000;
        else
            d = -0x100000;
        *(u32 *)(e + 0x9c) = *(u32 *)(f + 0x24) + d;
        *(u32 *)(e + 0xa0) = *(u32 *)(f + 0x28);
        func_ov002_0204d270(v, *(u8 *)(e + 1), 0x32);
        func_ov002_02078208(a0);
        *(u8 *)(e + 0xd1) = 4;
        ret = 1;
    }
    return ret;
}