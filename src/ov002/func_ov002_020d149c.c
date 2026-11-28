#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_02078208(s32 a0);
extern u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

u32 func_ov002_020d149c(s32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    s32 off = a0 * 0x158;
    u8 *g = base + 0x1824;
    u32 t = *(u8 *)(base + off + 0x2e6);
    u8 *e = base + 0x2e4 + off;
    Vec3 q;
    s32 delta;
    s32 mode;
    u32 r = 0;

    if (*(u32 *)(e + 0xd8) & 2) {
        *(u16 *)(e + 0x66) = 0;
        q = *(Vec3 *)(g + 0x24);
        if (q.x < *(s32 *)(e + 4)) {
            delta = 0x160000;
            mode = 0x2f;
        } else {
            delta = -0x160000;
            mode = 0x2e;
        }
        *(u16 *)(e + 0x8e) = *(u16 *)(g + 0xac);
        *(u16 *)(e + 0x8c) = *(u16 *)(g + 0xac);
        *(u32 *)(e + 0x90) = *(u32 *)(e + 4);
        *(u32 *)(e + 0x94) = *(u32 *)(e + 8);
        *(s32 *)(e + 0x9c) = q.x + delta;
        *(u32 *)(e + 0xa0) = q.y;
        func_ov002_02078208(a0);
        func_ov002_0204d270(t, *(u8 *)(e + 1), (u8)mode);
        *(u8 *)(e + 0xd1) = 2;
        r = 1;
    }
    return r;
}