#include "types.h"

extern u8 data_ov002_02102070[];

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

s32 func_02034148(s32 a0);
void func_ov002_0204d350(u32 a0, u32 a1, u32 a2);

void func_ov002_02057f48(u32 id, Vec3 pos, Vec3 rot, u32 a7) {
    u8 *base = *(u8 **)data_ov002_02102070 + 0x2e4;
    u8 *ent = base + id * 0x158;
    s32 dx;
    s32 dy;
    u32 dir;

    rot.x = rot.x + ((rot.x > 0) ? -0x140000 : 0x140000);
    *(u16 *)(ent + 0x66) = 0;
    *(u16 *)(ent + 0x8c) = a7;
    *(Vec3 *)(ent + 0x90) = pos;
    *(Vec3 *)(ent + 0x9c) = rot;
    dx = rot.x - pos.x;
    dy = rot.y - pos.y;
    if (func_02034148(dy) > func_02034148(dx) * 2)
        dir = (dy < 0) ? 0x54 : 0x53;
    else
        dir = (dx < 0) ? 0x51 : 0x52;
    func_ov002_0204d350(ent[2], ent[1], dir & 0xff);
}