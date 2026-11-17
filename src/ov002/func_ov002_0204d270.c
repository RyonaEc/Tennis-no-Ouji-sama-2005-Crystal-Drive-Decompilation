#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern u32 func_ov002_0204d350(s32, s32, s32);

u32 func_ov002_0204d270(s32 a0, s32 a1, s32 a2)
{
    u32 r;
    u8 *t = B + 0x274;
    r = func_ov002_0204d350(a0, a1, a2);
    *(u32 *)(t + 0x18 + a0 * 0x1c) |= 0x20;
    return r;
}
