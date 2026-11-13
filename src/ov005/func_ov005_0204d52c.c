#include "types.h"

extern u32 func_02026d54(u32, u32);
extern u8 data_ov005_020c9744[];

u32 func_ov005_0204d52c(void)
{
    u32 t0 = func_02026d54((*(u32 *)((u8 *)data_ov005_020c9744) + 0x98000), (*(u32 *)((u8 *)data_ov005_020c9744) + 0xb0000));
    return 0x0;
}
