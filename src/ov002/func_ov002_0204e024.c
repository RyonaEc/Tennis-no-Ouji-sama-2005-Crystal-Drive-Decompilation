#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0204e024(u32 a0)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e34);
    s32 v = a0 * 2 - 1;
    p[0x32] = -v;
    p[0x33] = 0x20;
}
