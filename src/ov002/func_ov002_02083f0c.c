#include "types.h"

extern u8 data_ov002_02102070[];

s32 func_ov002_02083f0c(u32 a0)
{
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *b = base + 0x36b3;

    *(u8 *)(b + a0) = *(u8 *)(b + a0) + 1;
    switch (*(u8 *)(base + 0x8f)) {
    case 0:
        return 1;
    case 1:
        return *(u8 *)(b + a0) == 2;
    case 2:
        return *(u8 *)(b + a0) == 3;
    default:
        return 0;
    }
}