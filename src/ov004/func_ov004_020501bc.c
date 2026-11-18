#include "types.h"

s32 func_ov004_020501bc(u8 *o, s32 v)
{
    s32 n = *(s32 *)o;
    if (n >= 0x10) return 0;
    *(s32 *)(o + n * 4 + 0x20) = v;
    (*(s32 *)o)++;
    return 1;
}