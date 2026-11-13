#include "types.h"

extern u8 data_02042a68[];

u32 func_0200fb50(void)
{
    return *(u16 *)(*(u32 *)(*(u32 *)((u8 *)data_02042a68 + 16) + 340) + 4);
}
