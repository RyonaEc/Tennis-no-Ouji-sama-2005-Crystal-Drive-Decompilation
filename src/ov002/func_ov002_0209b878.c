#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0204d22c(u32, u32, u32);
s32 func_ov002_0209b878(u32 a0)
{
    u8 *p = B + 0x2e4 + a0 * 0x158;
    u8 *q = B;
    s32 r = 0;
    u8 k = *(u8 *)(q + a0 * 0x158 + 0x2e6);

    if (*(u32 *)(p + 0xd8) & 0x10)
    {
        *(u16 *)(p + 0x66) = r;
        func_ov002_0204d22c(k, *(u8 *)(p + 1), 0x34);
        *(u8 *)(p + 0xd1) = 5;
        r = 1;
        *(u32 *)(q + 0x54 + a0 * 4) |= 8;
    }
    return r;
}
