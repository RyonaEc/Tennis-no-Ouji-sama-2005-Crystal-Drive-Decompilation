#include "types.h"

extern u32 data_ov002_02102070[];
extern void func_ov002_0206e140(s32 a0);

void func_ov002_02059000(s32 a0)
{
    u8 *p = (u8 *)data_ov002_02102070[0] + 0x2e4 + a0 * 0x158;

    if (p[0x12e] == 0)
        return;
    p[0x12e]--;
    if (p[0] != 0xd)
        return;
    func_ov002_0206e140(a0);
}