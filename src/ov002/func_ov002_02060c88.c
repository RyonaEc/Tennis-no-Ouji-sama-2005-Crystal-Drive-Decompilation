#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02060c88(void)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)(v0) + 53) = 0x0;
    return v0;
}
