#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02078208(s32 a0);
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);

u32 func_ov002_02092a5c(s32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 off = a0 * 0x158;
    u8 *g = base + 0x1824;
    u32 t = *(u8 *)(base + off + 0x2e6);
    u8 *e = base + 0x2e4 + off;
    u32 r = 0;

    if (*(u32 *)(e + 0xd8) & 8) {
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = *(u16 *)(g + 0xac);
        *(u16 *)(e + 0x8c) = *(u16 *)(g + 0xac);
        if (*(u16 *)(e + 0x8c) > 0x2b)
            *(u16 *)(e + 0x8c) = 0x2b;
        *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
        *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
        *(u32 *)(e + 0x9c) = *(u32 *)(g + 0x24) + (*(u8 *)(e + 2) < 2 ? -0x100000 : 0x100000);
        *(u32 *)(e + 0xa0) = *(u32 *)(g + 0x28);
        *(u32 *)(e + 0xb0) = 0x1e79e;
        *(u32 *)(e + 0xbc) = 0xffffe8c8;
        func_ov002_02078208(a0);
        func_ov002_0204d270(t, *(u8 *)(e + 1), 0x2d);
        *(u8 *)(e + 0xd1) = 4;
        r = 1;
    }
    return r;
}