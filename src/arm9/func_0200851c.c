#include "types.h"

extern u8 data_0203bbec[];

u32 func_0200851c(u32 a0)
{
    return *(u32 *)((u8 *)((data_0203bbec + (a0 << 0x3))));
}
