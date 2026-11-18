#include "types.h"

void func_020269b8(u16 v, u8 *dst, s32 count)
{
    s32 i;

    for (i = 0;; i += 2) {
        if (i >= count)
            return;
        *(u16 *)(dst + i) = v;
    }
}