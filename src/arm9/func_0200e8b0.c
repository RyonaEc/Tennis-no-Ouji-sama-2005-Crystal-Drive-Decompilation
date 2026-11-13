#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
u32 func_0200e8b0(u32 a0, u32 a1)
{
    if (*(u8 *)(data_0203d06c + a0 * 0x18) & 4) a0--;
    if (*(u8 *)(data_0203d06c + a1 * 0x18) & 4) a1--;
    return a0 == a1;
}
