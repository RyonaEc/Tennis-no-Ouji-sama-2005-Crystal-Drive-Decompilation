#include "types.h"

void func_ov002_020bcf9c(u8 *a0)
{
    s32 h, v, t;
    *(u32 *)(a0 + 4) = *(u32 *)(a0 + 0x9c);
    *(u32 *)(a0 + 8) = *(u32 *)(a0 + 0xa0);
    h = *(s16 *)(a0 + 0x66);
    v = *(u32 *)(a0 + 0xc8);
    t = v * h;
    *(u32 *)(a0 + 0xc) = *(u32 *)(a0 + 0xa4) + (h * t) / 2;
    if (*(s32 *)(a0 + 0xc) < 0)
        *(u32 *)(a0 + 0xc) = 0;
}
