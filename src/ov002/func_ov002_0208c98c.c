#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_0208c98c(u32 a0, u32 a1, u16 a2, u16 a3)
{
    u8 *base = *(u8 **)data_ov002_02102070 + 0x4bc4;
    u8 *e;

    if (a0 >= 2)
        return;
    if (a1 >= 4)
        return;
    e = base + 0xba + a0 * 0x30 + a1 * 0xc;
    *(u16 *)(e + 4) = a2;
    *(u16 *)(e + 6) = a3;
}