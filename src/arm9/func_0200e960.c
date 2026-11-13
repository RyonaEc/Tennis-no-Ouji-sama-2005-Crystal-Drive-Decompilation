#include "types.h"

extern u8 data_02042a68[];
void func_0200e960(u8 a0, u8 a1, u8 a2, u8 a3)
{
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + 0x128) = a0;
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + 0x129) = a1;
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + 0x12a) = a2;
    *(u8 *)(*(u8 **)(data_02042a68 + 0x10) + 0x12b) = a3;
}
