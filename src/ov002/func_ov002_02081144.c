#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02081144(s32 a0, s32 a1)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + 0x3654;
    s32 v = a1;

    if (*(u8 *)(p + 0x8d) == 2)
        v = 0x640000;
    if (v > 0x640000)
        v = 0x640000;
    if (v < 0)
        v = 0;
    *(u32 *)(q + a0 * 4) = v;
}
