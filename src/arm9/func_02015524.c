#include "types.h"

extern u8 data_02042a68[];

s32 func_02015524(void)
{
    u8 *p = (u8 *)(*(u32 *)(data_02042a68 + 0x14));
    s32 *q = (s32 *)(p + 0xc);

    *q = *q + 1;
    if (*(s32 *)((*(u32 *)(data_02042a68 + 0x14)) + 0xc) >= 0x12c)
        return 1;
    return 0;
}