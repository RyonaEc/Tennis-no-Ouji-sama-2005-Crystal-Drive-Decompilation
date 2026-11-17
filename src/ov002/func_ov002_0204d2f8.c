#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_ov002_0204d350(s32, s32, s32);
#define B (*(u8 **)data_ov002_02102070)
s32 func_ov002_0204d2f8(s32 a0, s32 a1, s32 a2)
{
    u32 i;
    u8 *t;
    u8 *p;
    i = a0 * 0x1c;
    t = B + 0x274;
    p = t;
    p += i;
    if (a2 == *(u8 *)(p + 2)) {
        if (a1 == *(u8 *)(t + i)) return 1;
    }
    return func_ov002_0204d350(a0, a1, a2);
}
