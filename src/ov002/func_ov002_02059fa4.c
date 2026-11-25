#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];
#define B (*(u8 **)data_ov002_02102070)

u32 func_ov002_02059fa4(u32 a0, u8 *a1, u8 *a2)
{
    u8 *e = B + 0x2e4 + a0 * 0x158;
    u8 *t = data_ov002_020fb544 + *e * 0x34;

    if (!(*(u32 *)(a2 + 0x10) & 4))
        return 0;
    if (*(s8 *)(a2 + 1) != *(s8 *)(e + 0xea))
        return 0;
    if (*(s32 *)(a1 + 0x10) > *(s32 *)(e + 0x84) || *(s32 *)(a1 + 0x18) > *(s32 *)(t + 0x14) || *(s32 *)(a1 + 0x14) > *(s32 *)(t + 0x10))
        return 0;
    return 1;
}