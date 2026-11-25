#include "types.h"

void func_ov002_020eb4f8(u8 *p)
{
    s32 v;

    *(u32 *)(p + 0xb8) |= 8;
    *(u32 *)(p + 0xcc) = 0x143;
    *(s32 *)(p + 0x5c) = *(s32 *)(p + 0x5c) / 2;
    *(s32 *)(p + 0x60) = *(s32 *)(p + 0x60) / 2;
    *(u32 *)(p + 0x64) = 0xaaaa;
    *(u32 *)(p + 0x5c) = *(u32 *)(p + 0x5c) * 0xffffffffu;
    v = *(s32 *)(p + 4);
    if (v >= 0 && *(s32 *)(p + 0x10) <= 0) {
        if (v < 0x10000)
            *(s32 *)(p + 4) = 0x10000;
    } else if (v > -0x10000) {
        *(s32 *)(p + 4) = -0x10000;
    }
}