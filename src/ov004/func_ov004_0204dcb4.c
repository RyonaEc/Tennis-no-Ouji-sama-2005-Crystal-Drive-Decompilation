#include "types.h"

s32 func_ov004_0204dcb4(u8 *p, u32 v)
{
    if (*(u32 *)(p + 0x10) == 0)
        return 0;
    if (*(u32 *)p != 0)
        return 0;
    *(u32 *)(p + 4) = v;
    *(u32 *)p = 1;
    return 1;
}