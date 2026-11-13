#include "types.h"

s32 func_02017784(u8 *a, u8 *b)
{
    if (b == 0) {
        return *(s32 *)a;
    }
    return *(s32 *)(b + *(u16 *)(a + 0xa) + 4);
}
