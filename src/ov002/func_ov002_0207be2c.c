#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_0207be2c(u32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 v = *(u8 *)(base + 0x3d + a0);

    if (v == 0xc || v == 0xd || v == 0x11) {
        *(u8 *)(base + 0x3f + a0) = 0;
        *(u8 *)(base + 0x3d + a0) = 0;
        *(u8 *)(base + 0x43 + a0) = 0;
    }
}