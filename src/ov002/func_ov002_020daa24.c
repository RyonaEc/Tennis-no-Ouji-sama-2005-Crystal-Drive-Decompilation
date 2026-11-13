#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0204d22c(u32, u32, u32);
s32 func_ov002_020daa24(u32 a0)
{
    u8 *p = B + 0x2e4 + a0 * 0x158;
    s32 r = 0;
    u8 k = *(u8 *)(B + a0 * 0x158 + 0x2e6);
    if (*(u32 *)(p + 0xd8) & 1)
    {
        *(u16 *)(p + 0x66) = r;
        *(u16 *)(p + 0x8e) = *(u8 *)(p + 0x80);
        if (k < 2)
        {
            *(u32 *)(p + 0xac) = 0xc0000;
            *(u32 *)(p + 0xb8) = 0xffff5556;
        }
        else
        {
            *(s32 *)(p + 0xac) = -0xc0000;
            *(u32 *)(p + 0xb8) = 0xaaaa;
        }
        if (*(u8 *)(p + 0x7c) == 0)
            func_ov002_0204d22c(k, *(u8 *)(p + 1), 5);
        else
            func_ov002_0204d22c(k, *(u8 *)(p + 1), 8);
        r = 1;
        *(u8 *)(p + 0xd1) = r;
    }
    return r;
}
