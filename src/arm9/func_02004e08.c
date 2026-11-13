#include "types.h"

extern u8 data_02042a68[];
u32 func_02004e08(void)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x10);
    u32 s = *(u32 *)(p + 0x14c) * 0x343fd + 0x269ec3;
    *(u32 *)(p + 0x14c) = s;
    return (u16)(0x7fff & ((s32)*(u32 *)(p + 0x14c) >> 16));
}
