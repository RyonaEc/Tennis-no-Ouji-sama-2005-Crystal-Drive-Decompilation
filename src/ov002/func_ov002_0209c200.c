#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_0204d350(s32 a0, s32 a1, s32 a2);

void func_ov002_0209c200(s32 a0)
{
    u32 base = data_ov002_02102070[0];
    u8 *node = (u8 *)(base + 0x2e4 + a0 * 0x158);
    u32 id = *(u8 *)(node + 2);
    u8 *ent = (u8 *)(base + 0x274 + id * 0x1c);
    u8 *slot = (u8 *)(base + (id >> 1));

    if (*(u8 *)(slot + 0x3d) != 0x17)
        return;
    if (a0 != *(u8 *)(slot + 0x3f))
        return;
    if (*(u8 *)(ent + 1) != 0x33)
        return;
    if (*(u8 *)(base + 0x100) == 0)
        return;
    func_ov002_0204d350(id, *(u8 *)(node + 1), 0);
}