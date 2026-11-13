#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
s32 func_ov002_02080eac(u32 a0, s32 a1)
{
    s32 v = *(s32 *)(B + a0 * 4 + 0x3654);
    return (s8)(v >= a1);
}
