#include "types.h"

extern u8 data_02042a68[];

u32 func_0201502c(void)
{
    return *(u16 *)(*(u32 *)((u8 *)data_02042a68 + 20) + 30);
}
