#include "types.h"

extern u8 data_0203bbeb[];

u32 func_0200ec4c(u32 a0)
{
    return *(u8 *)((u8 *)((data_0203bbeb + (a0 << 0x3))));
}
