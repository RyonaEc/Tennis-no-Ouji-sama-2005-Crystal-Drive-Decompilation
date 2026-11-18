#include "types.h"

void func_ov002_0204f7c4(u8 *a0, s16 *a1, u8 *a2)
{
    *a1 = (s16)(((*(s16 *)(a0 + 6) + 0x100) << 8) / 512);
    if (*a1 < 0x80)
        *a2 = 3;
    else
        *a2 = 2;
    *a1 = *a1 + 1;
}