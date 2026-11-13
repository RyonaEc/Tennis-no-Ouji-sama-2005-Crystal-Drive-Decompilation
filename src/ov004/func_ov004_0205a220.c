#include "types.h"

extern u8 data_ov004_020690a0[];

u32 func_ov004_0205a220(void)
{
    return *(u16 *)((*(u32 *)((u8 *)data_ov004_020690a0) + 0xc00) + 120);
}
