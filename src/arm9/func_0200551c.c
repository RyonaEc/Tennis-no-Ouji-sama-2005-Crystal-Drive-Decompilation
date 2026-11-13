#include "types.h"

extern u32 func_02026a50(u32, u32, u32);
extern u8 data_02042a68[];

u32 func_0200551c(u32 a0)
{
    return func_02026a50((*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 16) + 348) + 0x6c), a0, 0x200);
}
