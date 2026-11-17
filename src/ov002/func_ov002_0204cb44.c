#include "types.h"

extern s32 func_ov002_0204f478(s32, s32, s32);
s32 func_ov002_0204cb44(u8 *a0, u8 *a1, s32 a2, s32 a3)
{
    s32 total = 0;
    s32 i;
    for (i = 0; i < *(u16 *)(a0 + 2); i++, a1 += 0x10) {
        func_ov002_0204f478(a2 + *(s32 *)a1, a3 + total, *(s32 *)(a1 + 4));
        total += *(s32 *)(a1 + 4);
    }
    return total;
}
