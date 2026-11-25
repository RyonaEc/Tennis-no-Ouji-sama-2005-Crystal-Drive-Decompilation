#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d22c(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02078208(s32 a0);
extern void func_ov002_020788a8(u8 *a0);

s32 func_ov002_020b0328(s32 a0)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *e = base + 0x2e4 + a0 * 0x158;
    u8 v = *(u8 *)(base + a0 * 0x158 + 0x2e6);
    s32 ret = 0;

    if ((*(u32 *)(e + 0xd8) & 8) != 0) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = *(u8 *)(e + 0x80);
        *(u8 *)(base + 0x79) = 1;
        if (*(u8 *)(e + 0x7c) == 0)
            func_ov002_0204d22c(v, *(u8 *)(e + 1), 5);
        else
            func_ov002_0204d22c(v, *(u8 *)(e + 1), 8);
        func_ov002_02078208(a0);
        *(u8 *)(e + 0xd1) = 4;
        ret = 1;
        func_ov002_020788a8(e);
    }
    return ret;
}