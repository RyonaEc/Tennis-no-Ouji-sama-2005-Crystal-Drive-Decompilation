#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_0207ffa8(s32 a0, s32 a1)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + 0x3654 + a0;
    s32 v = a1;

    if (*(u8 *)(p + 0x8d) == 2)
        v = 3;
    if (v > 3)
        v = 3;
    if (v < 0)
        v = 0;
    *(u8 *)(q + 0x30) = v;
    *(u8 *)(q + 0x32) = 0;
}
