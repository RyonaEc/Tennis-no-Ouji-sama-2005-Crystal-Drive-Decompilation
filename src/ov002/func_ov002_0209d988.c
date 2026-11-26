#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02063720(s32 a0, s32 a1);
extern void func_ov002_02052608(void);
extern void func_ov002_02052520(s32 a0, s32 a1, s32 a2);
extern void func_ov002_02057068(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
extern void func_ov002_0204f898(void);
extern void func_ov002_020780f8(void);

void func_ov002_0209d988(s32 a0)
{
    u8 *e;
    u8 *q;
    u8 *g;
    u8 half;
    s32 y = 0;
    u32 z;
    u8 b;

    g = *(u8 **)data_ov002_02102070;
    b = *(u8 *)(g + a0 * 0x158 + 0x2e6);
    e = g + 0x2e4 + a0 * 0x158;
    q = g + 0x1744 + a0 * 0x38;
    half = b / 2;

    if (half == 0)
        z = 0x400000;
    else
        z = -0x400000;

    if (*(u32 *)(e + 0x4c) & 4) {
        func_ov002_02063720(a0, half);
        if (*(u8 *)(q + 8) == 2)
            y += 0x500000;
        if (*(u8 *)(q + 8) == 0)
            y -= 0x500000;
        if (*(u8 *)(q + 9) == 0)
            z += 0x3c0000;
        if (*(u8 *)(q + 9) == 2)
            z -= 0x3c0000;
    } else {
        u16 f = *(u16 *)(e + 0x50);
        if (f & 0x10)
            y += 0x500000;
        if (f & 0x20)
            y -= 0x500000;
        if (f & 0x80)
            z += 0x3c0000;
        if (f & 0x40)
            z -= 0x3c0000;
    }

    func_ov002_02052608();
    func_ov002_02052520(y, z, 0xc80000);
    func_ov002_02057068(0, 0, -0x60000, 0, 0, 0, half, 0x40000000, 0x39, *(u8 *)e, *(u16 *)(e + 0x12c), a0);
    func_ov002_0204f898();
    func_ov002_020780f8();
    *(u32 *)(g + 0x38) &= ~1;
}
