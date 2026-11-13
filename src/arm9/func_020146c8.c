#include "types.h"

extern u8 data_02042a68[];

u32 func_020146c8(u32 a0)
{
    *(u8 *)(*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 20) + 260) + 1) = a0;
    return a0;
}
