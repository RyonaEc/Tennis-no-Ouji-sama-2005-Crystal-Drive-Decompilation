#include "types.h"

extern u8 data_0203c678[];

u32 func_0200f45c(u32 a0)
{
    return *(u8 *)((u8 *)((data_0203c678 + (a0 << 0x2))));
}
