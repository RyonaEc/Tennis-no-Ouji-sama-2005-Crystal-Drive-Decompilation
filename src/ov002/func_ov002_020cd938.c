#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02078208(s32 a0);

s32 func_ov002_020cd938(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u8 v = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    u8 *f = base + 0x1824;
    s32 ret = 0;

    if ((*(u32 *)(e + 0xd8) & 0x10) != 0) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = *(u16 *)(f + 0xac);
        *(u16 *)(e + 0x8c) = *(u16 *)(f + 0xac) - 0xf;
        *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
        *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
        *(u32 *)(e + 0x9c) = *(u32 *)(f + 0x24);
        *(u32 *)(e + 0xa0) = *(u32 *)(f + 0x28);
        func_ov002_0204d270(v, *(u8 *)(e + 1), 0x30);
        func_ov002_02078208(a0);
        *(u8 *)(e + 0xd1) = 5;
        ret = 1;
    }
    return ret;
}