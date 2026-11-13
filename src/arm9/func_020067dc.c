#include "types.h"

extern u8 data_02042a68[];
extern s32 func_0201bd74(s32);

s32 func_020067dc(s32 v)
{
    u8 *p = *(u8 **)(data_02042a68 + 8);

    *(volatile s32 *)(p + 0x9c) = v;
    return func_0201bd74(*(volatile s32 *)(p + 0x9c));
}
