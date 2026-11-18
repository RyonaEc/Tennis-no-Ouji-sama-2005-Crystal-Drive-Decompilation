#include "types.h"

s32 func_0202f404(u8 *a0, u32 a1, s32 a2, s32 a3)
{
    s32 i;
    u16 mask = 1;

    for (i = 0; i < a3; i++) {
        if (a1 & mask)
            a2 += *(u16 *)(a0 + 0x810);
        mask <<= 1;
    }
    return a2;
}
