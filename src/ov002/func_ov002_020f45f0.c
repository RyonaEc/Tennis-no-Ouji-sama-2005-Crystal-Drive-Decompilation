#include "types.h"

extern u8 data_ov002_02102070[];
#define B (*(u8 **)data_ov002_02102070)
extern void func_ov002_0204d22c(u32, u32, u32);
s32 func_ov002_020f45f0(u32 a0)
{
    u8 *p = B + 0x2e4 + a0 * 0x158;
    u32 *slot = (u32 *)(p + 0x98);
    s32 r = 0;
    u8 k = *(u8 *)(B + a0 * 0x158 + 0x2e6);
    if (*(u32 *)(p + 0xd8) & 2)
    {
        *slot = 1;
        *(u16 *)(p + 0x66) = r;
        *(u16 *)(p + 0x8e) = 0x32;
        *(u16 *)(p + 0x8c) = *(u8 *)(p + 0x80);
        func_ov002_0204d22c(k, *(u8 *)(p + 1), 0x2f);
        *(u8 *)(p + 0xd1) = 2;
        r = 1;
    }
    return r;
}