#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];

s32 func_ov002_0205a060(s32 a0, u8 *a1, u8 *a2)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *e = p + 0x2e4 + a0 * 0x158;
    u8 *tab = data_ov002_020fb544 + *(u8 *)e * 0x34;

    if (!(*(u32 *)(a2 + 0x10) & 4))
        return 0;
    if (*(s8 *)(a2 + 1) != *(s8 *)(e + 0xea))
        return 0;
    if (*(s32 *)(a1 + 0x10) > *(s32 *)(e + 0x88) ||
        *(s32 *)(a1 + 0x18) > *(s32 *)(tab + 0x20) ||
        *(s32 *)(a1 + 0x1c) > -*(s32 *)(tab + 8) / 2 ||
        *(s32 *)(a1 + 0x14) > *(s32 *)(tab + 0x1c))
        return 0;
    return 1;
}