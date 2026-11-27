#include "types.h"

extern u8 *data_ov002_02102070;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

void func_ov002_02078208(s32 a0);
u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);
void func_ov002_020788a8(u8 *a0);

s32 func_ov002_020d1234(s32 id) {
    u8 *base = data_ov002_02102070;
    u8 b = *(u8 *)(base + id * 0x158 + 0x2e6);
    u8 *ent = base + 0x2e4 + id * 0x158;
    u8 *g = base + 0x1824;
    Vec3 v;
    s32 r = 0;
    s32 off;

    if (*(u32 *)(ent + 0xd8) & 0x10) {
        *(u16 *)(ent + 0x66) = 0;
        v = *(Vec3 *)(g + 0x24);
        off = (b < 2) ? 0x160000 : -0x160000;
        *(u16 *)(ent + 0x8e) = *(u16 *)(g + 0xac);
        *(u16 *)(ent + 0x8c) = *(u16 *)(g + 0xac);
        *(u32 *)(ent + 0x90) = *(u32 *)(ent + 4);
        *(u32 *)(ent + 0x94) = *(u32 *)(ent + 8);
        *(u32 *)(ent + 0x9c) = v.x + off;
        *(u32 *)(ent + 0xa0) = v.y;
        func_ov002_02078208(id);
        func_ov002_0204d270(b, *(u8 *)(ent + 1), 0x32);
        *(u8 *)(ent + 0xd1) = 5;
        r = 1;
        func_ov002_020788a8(ent);
    }
    return r;
}