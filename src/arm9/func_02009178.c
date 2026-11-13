#include "types.h"

extern u8 data_02042a68[];
void func_02009178(u32 a0)
{
    u32 *p = (u32 *)(*(u8 **)(data_02042a68 + 0x10));
    *p = (*p & ~2) | ((a0 & 1) << 1);
}
