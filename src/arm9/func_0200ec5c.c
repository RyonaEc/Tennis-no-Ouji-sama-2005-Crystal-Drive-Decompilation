#include "types.h"

extern u8 data_0203d06f[];

u32 func_0200ec5c(u32 a0)
{
    return *(u8 *)((u8 *)((data_0203d06f + (a0 * 0x18))));
}
