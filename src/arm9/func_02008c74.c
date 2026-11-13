#include "types.h"

extern u8 data_02042a68[];
void func_02008c74(u32 a0, u32 a1)
{
    *(u16 *)(*(u8 **)(data_02042a68 + 0x10) + 0xc) = a0;
    *(u16 *)(*(u8 **)(data_02042a68 + 0x10) + 0xe) = a1;
}
