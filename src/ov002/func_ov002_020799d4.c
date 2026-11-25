#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_020799d4(u32 a0)
{
    u8 *p = *(u8 **)data_ov002_02102070 + 0x2e4;
    u8 *q;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (*p != 0) {
            q = *(u8 **)data_ov002_02102070 + 0x274 + *(u8 *)(p + 2) * 0x1c;
            if (i == (s32)a0)
                *(u32 *)(q + 0x18) |= 0x20;
            else
                *(u32 *)(q + 0x18) &= ~0x20;
        }
        p += 0x158;
    }
}