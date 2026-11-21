#include "types.h"

extern u8 data_02042a68[];

void func_02015068(u32 a0)
{
    u8 *p = (u8 *)(*(u32 *)(data_02042a68 + 0x14));
    u16 *q = (u16 *)(p + 0x1e);

    *q |= 1 << a0;
}