#include "types.h"

extern u8 data_02042a68[];
void func_020051b8(u32 a0, u32 a1)
{
    u8 *p = *(u8 **)(data_02042a68 + 0xc);
    *(u32 *)p = a0;
    *(u32 *)(p + 4) = a1;
    *(u16 *)(p + 0xc) = 0;
    *(u32 *)(p + 0x10) = -1;
    *(u32 *)(p + 0x14) = *(u32 *)(p + 0x10);
}
