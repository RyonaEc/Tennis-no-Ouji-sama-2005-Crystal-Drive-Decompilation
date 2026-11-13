#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02060498(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)((v0 + a0)) + 338) = 0x1;
    return (v0 + a0);
}
