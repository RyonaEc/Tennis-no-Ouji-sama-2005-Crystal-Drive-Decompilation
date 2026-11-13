#include "types.h"

extern s16 data_ov002_020fc208[];
void func_ov002_0204f770(s32 a0, u32 a1, s32 *a2)
{
    u32 k = a1 & 0xff;
    a2[0] = (data_ov002_020fc208[k] * a0) >> 8;
    a2[1] = (data_ov002_020fc208[k + 0x40] * a0) >> 8;
    a2[2] = 0;
}
