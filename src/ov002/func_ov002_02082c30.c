#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02082c30(u8 a0, u8 a1, u16 a2)
{
    u8 *p = *(u8 **)data_ov002_02102070 + 0x36ac;

    *(u8 *)(p + 0x17) = a1;
    *(u16 *)(p + 0x1a) = a2;
    *(u16 *)(p + 0x1c) = 0;
    *(u8 *)(p + 0x1e) = a0;
    *(u8 *)(p + 0x24) = 0xc;
}