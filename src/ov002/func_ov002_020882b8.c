#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_020882b8(u32 a0, s32 a1)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    if (a1 < 2)
        *(u32 *)(b + a1 * 4 + 0x1c) = a0;
}
