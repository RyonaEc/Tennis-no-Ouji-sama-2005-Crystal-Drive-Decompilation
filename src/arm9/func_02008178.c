#include "types.h"

extern u8 data_0203c0f4[];

s32 func_02008178(s32 a0, s32 a1)
{
    s32 v;
    s32 i;

    if (a0 != 0xb)
        return *(s32 *)(data_0203c0f4 + a0 * 12);
    if (a1 == 0)
        return 0;
    v = 0x7d0;
    for (i = 1; i < a1; i++) {
        v <<= 1;
        if (v >= 0x3b9ac9ff)
            return 0x3b9ac9ff;
    }
    return v;
}