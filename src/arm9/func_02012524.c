#include "types.h"

extern u8 data_02042a68[];

u32 func_02012524(u32 a0)
{
    *(u32 *)((u8 *)data_02042a68 + 28) = a0;
    return a0;
}
