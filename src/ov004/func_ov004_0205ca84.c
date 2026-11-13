#include "types.h"

extern u8 data_ov004_020690a0[];

u32 func_ov004_0205ca84(void)
{
    return *(u16 *)((*(u32 *)((u8 *)data_ov004_020690a0) + 0x1100) + 4);
}
