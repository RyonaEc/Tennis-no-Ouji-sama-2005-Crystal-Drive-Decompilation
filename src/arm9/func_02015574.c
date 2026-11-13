#include "types.h"

extern u8 data_02042a68[];

u32 func_02015574(void)
{
    return ((*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 20)) << 0x1c) >> 0x1f);
}
