#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_0204d22c(u32 a0, u32 a1, u32 a2);
extern u32 func_ov002_020788a8(u32 a0);

s32 func_ov002_020b5b74(s32 a0)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    s32 off = a0 * 0x158;
    u8 *e = p + 0x2e4 + off;
    u8 *c = p + 0x1e34;
    s32 r = 0;
    u32 m;

    m = *(u8 *)(p + off + 0x2e6);
    if (*(u32 *)(e + 0xd8) & 0x20) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = *(u8 *)(e + 0x80);
        p[0x79] = 1;
        *(u32 *)(p + 0x80) = *(u32 *)(c + 0x2c);
        if (m < 2)
            *(u32 *)(p + 0x84) = -0x80000;
        else
            *(u32 *)(p + 0x84) = 0x80000;
        *(u32 *)(p + 0x88) = 0xd8000;
        if (*(u8 *)(e + 0x7c) == 0)
            func_ov002_0204d22c(m, *(u8 *)(e + 1), 0x33);
        else
            func_ov002_0204d22c(m, *(u8 *)(e + 1), 0x34);
        *(u8 *)(e + 0xd1) = 6;
        r = 1;
        func_ov002_020788a8((u32)e);
    }
    return r;
}