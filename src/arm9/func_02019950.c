#include "types.h"

extern u8 data_0204662c[];

u32 func_02019950(void)
{
    *(u32 *)((u8 *)(0x4000448)) = 0x1;
    *(u32 *)(data_0204662c) = 0x0;
    return (u32)data_0204662c;
}
