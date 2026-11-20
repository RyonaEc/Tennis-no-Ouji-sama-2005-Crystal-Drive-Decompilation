#include "types.h"

extern u8 data_0204aec0[];

s32 func_0202cc74(void)
{
    return *(u32 *)(data_0204aec0 + 0x34) != 0;
}