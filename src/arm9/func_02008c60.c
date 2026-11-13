#include "types.h"

extern u8 data_02042a68[];

u32 func_02008c60(void)
{
    return *(u8 *)(*(u32 *)((u8 *)data_02042a68 + 16) + 9);
}
