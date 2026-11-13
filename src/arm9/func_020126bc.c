#include "types.h"

extern u8 data_02042a68[];

u32 func_020126bc(u32 a0)
{
    *(u32 *)((u8 *)data_02042a68 + 24) = a0;
    return a0;
}
