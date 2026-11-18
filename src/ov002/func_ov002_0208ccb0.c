#include "types.h"

extern void func_ov002_0204f5a0(void *, void *, s32);

s32 func_ov002_0208ccb0(u8 *a0, u8 *a1, u8 *a2, u8 *a3)
{
    s32 total = 0;
    s32 i;

    for (i = 0; i < *(u16 *)(a0 + 2); i++) {
        if (*(s32 *)(a1 + 8) == 1) {
            func_ov002_0204f5a0(a2 + *(u32 *)a1, a3 + total, *(s32 *)(a1 + 4));
            total += *(s32 *)(a1 + 4);
        }
        a1 += 0x14;
    }
    return total;
}
