#include "types.h"

extern u32 func_ov004_0204d9bc(u32, u32);
extern u8 data_ov004_020690a0[];

u32 func_ov004_0205a75c(void)
{
    u32 t0;
    if (*(u32 *)((u8 *)((*(u32 *)(data_ov004_020690a0) + 0x1000)) + 940) == 0x0) return 0x0;
    if (*(u32 *)((u8 *)(*(u32 *)((u8 *)((*(u32 *)(data_ov004_020690a0) + 0x1000)) + 940)) + 16) == 0x0) return 0x0;
    t0 = func_ov004_0204d9bc(*(u32 *)((u8 *)((*(u32 *)(data_ov004_020690a0) + 0x1000)) + 940), *(u32 *)((u8 *)(*(u32 *)((u8 *)((*(u32 *)(data_ov004_020690a0) + 0x1000)) + 940)) + 16));
    return t0;
}
