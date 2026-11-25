#include "types.h"

extern u32 data_ov002_02102070[];

void func_ov002_0204f898(void)
{
    u8 *base = (u8 *)data_ov002_02102070[0];
    u8 *f = base + 0x1824;
    u8 b = *(u8 *)(base + 0x4f21);

    *(u8 *)(f + 0x10c) = b;
    if (*(u8 *)(f + 0x10c) == 0)
        *(u32 *)(f + 0xcc) += 1;
}