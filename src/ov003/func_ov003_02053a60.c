#include "types.h"

extern u8 data_ov003_020dc178[];

s32 func_ov003_02053a60(u32 a0)
{
    u32 k;

    k = *(s16 *)(data_ov003_020dc178 + 0x14);
    return (s16)(a0 * 45 * k / 100 - a0 * 9 * a0 / 2);
}
