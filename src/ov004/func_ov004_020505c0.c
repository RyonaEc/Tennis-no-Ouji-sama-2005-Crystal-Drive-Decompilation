#include "types.h"

extern u32 func_ov004_020528c0(u32 *p);

u32 func_ov004_020505c0(s32 a0, u32 *a1)
{
    s32 v;

    if (func_ov004_020528c0(a1) == 0)
        return 0;
    v = 0;
    switch (a0) {
    case 0:
        v = 5;
        break;
    case 0x13:
        v = 3;
        break;
    case 0x25:
        v = 2;
        break;
    case 0x28:
        v = -1;
        break;
    case 0x32:
        v = 8;
        break;
    case 0x3a:
        v = 4;
        break;
    }
    *(u32 *)((u8 *)a1 + 0x74) += (u32)v << 12;
    return 1;
}