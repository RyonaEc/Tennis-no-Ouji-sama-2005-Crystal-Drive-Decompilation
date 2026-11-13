#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
u32 func_02010150(u32 a0)
{
    if (*(u8 *)(data_0203d06c + a0 * 0x18) & 4) return 0;
    return *(u8 *)(*(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154) + a0 + 0x14) != 0;
}
