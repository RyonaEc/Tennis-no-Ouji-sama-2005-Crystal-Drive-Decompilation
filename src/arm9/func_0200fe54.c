#include "types.h"

extern u8 data_02042a68[];
u32 func_0200fe54(u32 a0)
{
    u8 *p = (u8 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + a0);
    return *(u8 *)(p + 0x84) >= 3;
}
