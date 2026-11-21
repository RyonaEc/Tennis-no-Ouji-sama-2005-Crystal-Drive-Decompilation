#include "types.h"

extern u8 data_0204ab00[];

u32 func_020279e0(u32 a0, u32 a1, u32 a2)
{
    u8 *p = data_0204ab00 + a0 * 12;

    *(u32 *)p = a1;
    *(u32 *)(p + 4) = a2;
    (*(u8 *)(p + 8))++;
    return *(u8 *)(p + 8);
}