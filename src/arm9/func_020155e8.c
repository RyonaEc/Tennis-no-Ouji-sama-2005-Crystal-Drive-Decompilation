#include "types.h"

extern u8 data_02042a68[];
void func_020155e8(void)
{
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) &= ~4;
    *(u32 *)(*(u8 **)(data_02042a68 + 0x14)) |= 2;
}
