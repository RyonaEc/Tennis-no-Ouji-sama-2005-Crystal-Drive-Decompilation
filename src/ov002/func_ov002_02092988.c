#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_02078208(s32 a0);
u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2);

s32 func_ov002_02092988(s32 id) {
    u8 *base = data_ov002_02102070;
    u8 b = *(u8 *)(base + id * 0x158 + 0x2e6);
    u8 *ent = base + 0x2e4 + id * 0x158;
    u8 *g = base + 0x1824;
    s32 r = 0;
    s32 off;

    if (*(u32 *)(ent + 0xd8) & 0x10) {
        *(u16 *)(ent + 0x66) = 0;
        *(u16 *)(ent + 0x8e) = *(u16 *)(g + 0xac);
        *(u16 *)(ent + 0x8c) = *(u16 *)(g + 0xac);
        if (*(u16 *)(ent + 0x8c) > 0x2b)
            *(u16 *)(ent + 0x8c) = 0x2b;
        *(u32 *)(ent + 0x90) = *(u32 *)(ent + 4);
        *(u32 *)(ent + 0x94) = *(u32 *)(ent + 8);
        off = (*(u8 *)(ent + 2) < 2) ? -0x100000 : 0x100000;
        *(u32 *)(ent + 0x9c) = *(u32 *)(g + 0x24) + off;
        *(u32 *)(ent + 0xa0) = *(u32 *)(g + 0x28);
        func_ov002_02078208(id);
        func_ov002_0204d270(b, *(u8 *)(ent + 1), 0x2e);
        *(u8 *)(ent + 0xd1) = 5;
        r = 1;
    }
    return r;
}