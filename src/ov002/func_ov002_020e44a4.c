#include "types.h"

extern u8 data_ov002_02102070[];
u32 func_ov002_020e44a4(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u32 v = *(u32 *)(b + a0 * 4 + 0x54);
    if (v & 0x10) return 1;
    if (v & 0x20) return 2;
    return 0;
}
