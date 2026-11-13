#include "types.h"

extern u8 data_ov003_020dc744[];

u32 func_ov003_020641e4(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)((u8 *)((data_ov003_020dc744 + (a0 << 0x2))));
    *(u32 *)((u8 *)(0x4000484)) = v0;
    return v0;
}
