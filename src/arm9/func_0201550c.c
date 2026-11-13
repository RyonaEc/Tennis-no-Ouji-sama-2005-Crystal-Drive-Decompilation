#include "types.h"

extern u8 data_02042a68[];

u32 func_0201550c(void)
{
    u32 v0;
    v0 = *(u32 *)(data_02042a68 + 20);
    *(u32 *)((u8 *)(v0) + 12) = 0x0;
    return v0;
}
