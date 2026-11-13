#include "types.h"

extern u8 data_02042a68[];

u32 func_02010340(u32 a0)
{
    *(u32 *)(*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 16) + 344)) = a0;
    return a0;
}
