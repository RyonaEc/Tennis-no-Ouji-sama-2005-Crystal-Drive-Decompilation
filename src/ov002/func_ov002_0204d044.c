#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
s32 func_ov002_0204d044(u32 a0, u16 a1, u16 a2, u16 a3, u8 a4)
{
    u8 *t = B + 0x274;
    u8 *p;
    if (a0 >= 4) return 0;
    p = t + a0 * 0x1c;
    *(u16 *)(p + 8) = a1;
    *(u16 *)(p + 0xa) = a2;
    *(u16 *)(p + 0xc) = a3;
    *(u8 *)(p + 0xe) = a4;
    return 1;
}
