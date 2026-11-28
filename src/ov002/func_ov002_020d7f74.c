#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02078208(s32 a0);
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

u32 func_ov002_020d7f74(s32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 off = a0 * 0x158;
    u8 *h = base + 0x1824;
    u8 *g = base + 0x1934;
    u32 t = *(u8 *)(base + off + 0x2e6);
    u8 *e = base + 0x2e4 + off;
    Vec3 v;
    s32 k;
    s32 d;
    u32 r = 0;

    if (*(u32 *)(e + 0xd8) & 1) {
        k = *(u16 *)(h + 0xaa);
        v = *(Vec3 *)(g + k * 0x14 + 4);
        *(u16 *)(e + 0x66) = 0;
        *(u16 *)(e + 0x8e) = k;
        *(u16 *)(e + 0x8c) = k;
        d = (*(u8 *)(e + 2) < 2) ? 0x140000 : -0x140000;
        *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
        *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
        *(s32 *)(e + 0x9c) = v.x + d;
        *(u32 *)(e + 0xa0) = v.y;
        func_ov002_02078208(a0);
        func_ov002_0204d270(t, *(u8 *)(e + 1), 0x2d);
        *(u8 *)(e + 0xd1) = 1;
        r = 1;
    }
    return r;
}