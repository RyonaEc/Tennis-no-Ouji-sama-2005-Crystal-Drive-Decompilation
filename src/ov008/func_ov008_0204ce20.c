#include "types.h"

u32 func_ov008_0204ce20(void)
{
    u8 *p = *(u8 **)0x0204f440;
    u32 v = *(u32 *)(p + 0x3c);

    if (*(u8 *)(p + 0x34) != 0) {
        u8 *c = (u8 *)(p + 0x34);
        *c = *c - 1;
        return 0;
    }
    if (v & 0x400) {
        *(u8 *)(p + 0x34) = 8;
        return 1;
    }
    return 0;
}