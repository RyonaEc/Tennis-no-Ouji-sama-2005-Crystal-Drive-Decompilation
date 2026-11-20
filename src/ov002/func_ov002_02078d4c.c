#include "types.h"

extern u8 data_ov002_02102070[];

s32 func_ov002_02078d4c(void)
{
    u8 *b;
    u8 *p;
    u32 v;

    b = *(u8 **)data_ov002_02102070;
    p = b + 0x1824;
    if (*(u8 *)(b + 0x100) != 0)
        goto zero;
    v = *(u32 *)(p + 0xcc);
    if (v == 0x172)
        goto one;
    if (v == 0x174)
        goto one;
zero:
    return 0;
one:
    return 1;
}
