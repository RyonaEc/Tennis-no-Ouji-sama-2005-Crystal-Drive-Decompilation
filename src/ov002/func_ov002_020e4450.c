#include "types.h"

extern u8 data_ov002_02102070;

void func_ov002_020e4450(s32 idx, s32 mode)
{
    u32 *t = (u32 *)(*(u8 **)&data_ov002_02102070 + 0x54);
    u32 *p = &t[idx];

    *p = *p & ~0x30;
    switch (mode) {
    case 0:
        break;
    case 1:
        *p = *p | 0x10;
        break;
    case 2:
        *p = *p | 0x20;
        break;
    }
}