#include "types.h"

extern u8 *data_02047f00[];
u32 func_0201d48c(s32 idx)
{
    u8 *g = *data_02047f00;
    u8 *b = *(u8 **)(g + 0x84);
    u32 v = *(u32 *)(b + 0x10);

    if (v == 0) b = 0; else b = b + v;
    if (b == 0)
        return 0;
    if (idx < 0)
        return 0;
    if ((u32)idx >= *(u32 *)b)
        return 0;
    v = *(u32 *)(b + 4 + idx * 4);
    g = *(u8 **)(g + 0x84);
    if (v == 0) return 0;
    return (u32)(g + v);
}
