#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)

extern s32 func_02034148(u32);
extern s32 func_ov002_02062dbc(u32);

void func_ov002_02063bb8(u32 a0)
{
    u8 *p = B + 0x1744 + a0 * 0x38;
    u8 *s = B + 0x1824;
    s32 limit;
    s32 v;

    v = func_02034148(*(u32 *)(B + a0 * 0x158 + 0x2ec));
    if (v < 0x600000)
    {
        limit = 0x10;
        if (*(u32 *)(s + 0xb8) & 0x40000000) limit = 0x40;
        if (*(u32 *)(s + 0xb8) & 0x8000000) limit = 0;
    }
    else
        limit = 0;
    if (func_ov002_02062dbc(0x100) < limit)
        *(u32 *)(p + 0x24) |= 0x800000;
    else
        *(u32 *)(p + 0x24) &= ~0x800000;
}