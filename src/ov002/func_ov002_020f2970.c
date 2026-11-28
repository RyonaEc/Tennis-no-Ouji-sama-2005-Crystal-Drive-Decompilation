#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_020f2970(s32 a0)
{
    u8 *e;
    s32 *p;
    u8 *g = *(u8 **)data_ov002_02102070;

    e = g + 0x2e4 + a0 * 0x158;

    p = (s32 *)(0xac + e);

    if (*(u8 *)(e + 0xd1) == 2) {
        *(u8 *)(0x142 + g + a0) = 0;
        *p = *(u8 *)(g + a0 + 0x146);
    } else {
        *(u8 *)(0x14a + g + a0) = 0;
        *p = *(u8 *)(g + a0 + 0x14e);
    }

}