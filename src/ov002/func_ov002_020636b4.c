#include "types.h"

extern u8 *data_ov002_02102070;
extern s32 func_ov002_02062dbc(u32 a0);

void func_ov002_020636b4(u32 a0)
{
    u8 *base = data_ov002_02102070;
    u8 *q = base + 0x1744 + a0 * 0x38;

    if (*(u32 *)(base + 0x18dc) & 0x40000000)
    {
        if (func_ov002_02062dbc(0xa) == 0)
            *(u32 *)(q + 0x24) |= 0x10000000;
        else
            *(u32 *)(q + 0x24) &= ~0x10000000;
    }
}