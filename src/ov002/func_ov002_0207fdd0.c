#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_02080eac(s32 a0, s32 a1);

s32 func_ov002_0207fdd0(s32 a0)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    s32 n = *(u8 *)(p + a0 + 0x3684);

    if (func_ov002_02080eac(a0 & 0xff, 0x640000))
        n++;
    return n;
}
