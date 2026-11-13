#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern s32 func_ov002_02050300(s32);
extern s32 func_02034148(s32);
s32 func_ov002_0206394c(u32 a0)
{
    u8 *b = B;
    u8 *p = b + a0 * 0x158;
    u8 *q = b + 0x1824;
    s32 d = *(u32 *)(p + 0x2e8);
    d -= *(s32 *)(q + 0x18);
    if (func_ov002_02050300(*(s32 *)(q + 0x18)) == 1)
    {
        if (*(s32 *)(q + 0x5c) < 0) return 1;
        return -1;
    }
    if (func_02034148(d) < 0x200000) return 0;
    if (d < 0) return 1;
    return -1;
}
