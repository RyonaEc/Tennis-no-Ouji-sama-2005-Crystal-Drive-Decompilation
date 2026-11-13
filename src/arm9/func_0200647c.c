#include "types.h"

extern u32 func_0201f924(u32, u32, u32);
extern u8 data_02042a68[];

u32 func_0200647c(u32 a0)
{
    return func_0201f924((*(u32 *)((u8 *)data_02042a68 + 8) + 0x98), a0, (u32)data_02042a68);
}
