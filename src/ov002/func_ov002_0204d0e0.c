#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern u32 func_ov002_0204d2b4(s32, s32, s32);

u32 func_ov002_0204d0e0(s32 a0, s32 a1, s32 a2, s32 a3)
{
    u32 r;
    u8 *t = B + 0x274;
    r = func_ov002_0204d2b4(a0, a1, a3);
    *(u8 *)(t + a0 * 0x1c + 2) = a2;
    *(u32 *)(t + 0x18 + a0 * 0x1c) |= 0x20;
    return r;
}
