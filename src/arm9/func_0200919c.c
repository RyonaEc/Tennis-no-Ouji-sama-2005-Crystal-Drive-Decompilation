#include "types.h"

extern u8 data_02042a68[];
void func_0200919c(u32 a0, u8 a1, u8 a2, u8 a3)
{
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + a0 * 4 + 0x120) = a1;
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + a0 * 4 + 0x121) = a2;
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + a0 * 4 + 0x122) = a3;
}
