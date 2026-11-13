#include "types.h"

extern u8 data_02042a68[];

u32 func_020051a0(u32 a0, u32 a1)
{
    u32 v0;
    v0 = *(u32 *)(data_02042a68 + 12);
    *(u16 *)((u8 *)(v0) + 8) = a0;
    *(u16 *)((u8 *)(v0) + 10) = a1;
    return a0;
}
