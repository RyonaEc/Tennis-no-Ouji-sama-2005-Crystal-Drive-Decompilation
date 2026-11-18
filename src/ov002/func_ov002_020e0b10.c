#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0204d270(u32, u32, u32);
s32 func_ov002_020e0b10(u32 a0)
{
    u8 *p = B + 0x2e4 + a0 * 0x158;
    s32 r = 0;
    u8 k = *(u8 *)(B + a0 * 0x158 + 0x2e6);

    if (*(u32 *)(p + 0xd8) & 0x10)
    {
        *(u16 *)(p + 0x66) = r;
        *(u16 *)(p + 0x8e) = *(u8 *)(p + 0x80);
        if (*(u8 *)(p + 0x7c) == 0)
            func_ov002_0204d270(k, *(u8 *)(p + 1), 0x32);
        else
            func_ov002_0204d270(k, *(u8 *)(p + 1), 0x33);
        *(u8 *)(p + 0xd1) = 5;
        r = 1;
    }
    return r;
}
