#include "types.h"

extern u8 data_02042a68[];

u32 func_02008c28(void)
{
    return ((*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 16)) << 0x1d) >> 0x1f);
}
