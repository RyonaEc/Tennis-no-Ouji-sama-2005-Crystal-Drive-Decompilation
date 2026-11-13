#include "types.h"

extern u8 data_02042a68[];
u32 func_02012ea8(u32 a0)
{
    u8 *q = *(u8 **)(data_02042a68 + 0x14);
    if (*(u8 *)(q + a0 + 0x24) == 0xff) return 0xc4653601;
    return *(u32 *)(q + a0 * 4 + 0xe0);
}
