#include "types.h"

s32 func_ov004_02050154(u8 *p, s32 *out)
{
    s32 i;

    *out = -1;
    for (i = 0; i < *(s32 *)p; i++) {
        if (*(u32 *)(p + 0xc) == *(u32 *)(((u32 *)(p + 0x20))[i] + 0x98)) {
            *out = *(s32 *)(p + 0x18);
            return 1;
        }
    }
    return 0;
}