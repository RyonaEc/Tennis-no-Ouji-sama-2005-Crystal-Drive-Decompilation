#include "types.h"

extern u8 data_02042a68[];

u32 func_020138e8(u32 a0)
{
    *(u8 *)(*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 20) + 260) + 18) = a0;
    return a0;
}
