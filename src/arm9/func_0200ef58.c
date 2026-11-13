#include "types.h"

extern u8 data_02042a68[];
extern u8 data_0203d06c[];
u32 func_0200ef58(u32 a0)
{
    return (*(u8 *)(data_0203d06c + a0 * 0x18) & 4) != 0;
}
