#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
void func_0200ff70(u32 a0, u8 a1)
{
    if (*(u8 *)(data_0203d06c + a0 * 0x18) & 4)
        a0--;
    *(u8 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + a0 + 0x20c) = a1;
}
