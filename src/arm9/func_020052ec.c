#include "types.h"

extern u8 data_02042a68[];

u32 func_020052ec(u32 a0, u32 a1)
{
    *(u16 *)((u8 *)data_02042a68 + 40) = a0;
    *(u16 *)((u8 *)data_02042a68 + 42) = a1;
    return a0;
}
