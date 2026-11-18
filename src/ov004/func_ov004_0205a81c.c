#include "types.h"

extern u8 data_ov004_020690a0[];

s32 func_ov004_0205a81c(s32 i)
{
    u32 *b = (u32 *)(*(u8 **)data_ov004_020690a0 + 0x13a8);
    s32 *p = (s32 *)b[1];
    s32 *q;

    p = (s32 *)*p;
    if (p != 0)
        return 0;
    q = (s32 *)b[2];
    return q[i];
}