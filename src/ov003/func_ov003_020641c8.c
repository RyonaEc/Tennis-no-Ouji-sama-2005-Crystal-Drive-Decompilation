#include "types.h"

extern u8 data_ov003_020dc744[];
extern u32 func_02021d98(u32, u32, u32);
u32 func_ov003_020641c8(u32 a0, u32 a1)
{
    u32 v0;
    v0 = *(u32 *)((u8 *)((data_ov003_020dc744 + (a1 << 0x2))));
    return func_02021d98(a0, 0x21, v0);
}
