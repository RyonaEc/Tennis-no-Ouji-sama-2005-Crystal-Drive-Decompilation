#include "types.h"

extern u8 data_0203bbe8[];
extern u8 data_02042a68[];
u32 func_0200ff30(u32 a0)
{
    if (*(u8 *)(data_0203bbe8 + a0 * 8) & 2) return 0;
    return *(u8 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + a0 + 0x64) != 0;
}
