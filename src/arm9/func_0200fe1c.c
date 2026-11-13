#include "types.h"

extern u8 data_02042a68[];

u32 func_0200fe1c(u32 a0, u32 a1)
{
    u32 v0;
    v0 = *(u32 *)((u8 *)(*(u32 *)(data_02042a68 + 16)) + 340);
    *(u8 *)((u8 *)((v0 + a0)) + 132) = a1;
    return (v0 + a0);
}
