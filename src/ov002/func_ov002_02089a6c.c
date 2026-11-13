#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02089a6c(u32 a0)
{
    return *(u8 *)(((*(u32 *)((u8 *)data_ov002_02102070) + a0) + 0x4000) + 3859);
}
