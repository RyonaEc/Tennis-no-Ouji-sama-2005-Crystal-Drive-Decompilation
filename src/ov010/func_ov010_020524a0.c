#include "types.h"

s32 func_ov010_020524a0(u32 a0)
{
    s32 i;
    s32 n = 0;

    for (i = 1; i < 16; i++) {
        if (a0 & (1 << i))
            n++;
    }
    return n;
}
