#include "types.h"

extern u8 data_02042a68[];

u32 func_0200fe7c(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)((u8 *)(*(u32 *)(data_02042a68 + 16)) + 340);
    *(u8 *)((u8 *)((v0 + a0)) + 100) = 0x1;
    return (v0 + a0);
}
