#include "types.h"

extern u8 data_02042a68[];
void func_0200eaa4(u32 a0)
{
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + 9) = a0;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x10) + 0x14c) = *(u32 *)(data_02042a68 + 0x20);
}
