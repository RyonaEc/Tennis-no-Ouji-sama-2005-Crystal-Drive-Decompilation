#include "types.h"

extern u32 data_ov002_02102070[];

void func_ov002_0204f98c(void)
{
    u8 *f = (u8 *)data_ov002_02102070[0] + 0x1824;
    s32 dy;
    s32 dx;

    if ((*(u32 *)(f + 0xc8) & 0xc0) == 0)
        return;
    if (*(u8 *)(f + 0x8d) != 1)
        return;
    dx = *(s16 *)(f + 2) - *(s16 *)(f + 0x1a);
    dy = *(s16 *)(f + 6) - *(s16 *)(f + 0x1e);
    if (dx * dx + dy * dy > 0x1000)
        *(u32 *)(f + 0xc8) &= ~0xc0;
}