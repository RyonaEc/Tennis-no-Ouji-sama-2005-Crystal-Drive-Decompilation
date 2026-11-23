#include "types.h"

extern u8 data_ov002_02102070[];

void func_ov002_02077818(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    u8 *base = g + 0x1e6c;
    s32 i;
    u8 *p = base;

    for (i = 0; i < 10; i++, p += 0x50) {
        u16 f = *(u16 *)(p + 0x264);
        u32 n;

        if (!(f & 1))
            continue;
        if (!(f & 0x80))
            continue;
        n = *(u8 *)(p + 0x267);
        *(u8 *)(n + base + 0x584) = 0;
        n = *(u8 *)(p + 0x268);
        *(u32 *)(base + (n << 3) + 0x58c) &= ~(1u << *(u8 *)(p + 0x266));
        *(u16 *)(p + 0x264) = 0;
    }
}