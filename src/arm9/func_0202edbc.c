#include "types.h"

s32 func_0202edbc(u8 *a0)
{
    u32 v;
    s32 t;
    s32 i;
    s32 p;

    if (*(u16 *)(a0 + 4) > 0x70) return 0;
    v = *(u16 *)(a0 + 0x18);
    if (v < 0xa || v > 0x3e8) return 0;
    v = *(u16 *)(a0 + 0x32);
    if (v < 1 || v > 0xe) return 0;
    t = *(u16 *)(a0 + 0x36);
    i = *(u16 *)(a0 + 0x34);
    p = *(u16 *)(a0 + 0x10);
    return (p * (((t + 0x20) * 4) + 0x70) + (0x14a + (i + 0x26) * 4)) <= 0x15e0;
}