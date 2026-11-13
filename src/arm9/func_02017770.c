#include "types.h"

s32 func_02017770(u8 *a, u8 *b)
{
    if (b == 0) {
        return *(s32 *)(a + 4);
    }
    return *(s32 *)(b + *(u16 *)(a + 0xa));
}
