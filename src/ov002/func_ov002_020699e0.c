#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_020699e0(void)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)((v0 + 0x2000)) + 1180) = 0x0;
    return (v0 + 0x2000);
}
