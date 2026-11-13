#include "types.h"

extern u8 data_02042a68[];
void func_0200e928(u32 a0)
{
    u32 *p = (u32 *)(*(u8 **)(data_02042a68 + 0x10));
    *p = (*p & ~4) | ((a0 & 1) << 2);
}
