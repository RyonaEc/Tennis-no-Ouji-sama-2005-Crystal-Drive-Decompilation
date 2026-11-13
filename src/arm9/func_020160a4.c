#include "types.h"

extern u8 data_02042ae8[];

u32 func_020160a4(u32 a0)
{
    *(u16 *)((u8 *)data_02042ae8) = (a0 & 0x7);
    return (a0 & 0x7);
}
