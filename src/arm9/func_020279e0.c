#include "types.h"

extern u8 data_0204ab00[];

u8 func_020279e0(u32 i, u32 a, u32 b)
{
    u8 *p = data_0204ab00 + i * 12;
    u8 *q = p + 8;

    *(u32 *)p = a;
    *(u32 *)(p + 4) = b;
    *q = *q + 1;
    return p[8];
}