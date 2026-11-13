#include "types.h"

extern u32 func_020160a4(u32);
extern u8 data_02042a68[];

u32 func_0201518c(u32 a0)
{
    *(u16 *)(*(u32 *)((u8 *)data_02042a68 + 20) + 28) = a0;
    return func_020160a4(a0);
}
