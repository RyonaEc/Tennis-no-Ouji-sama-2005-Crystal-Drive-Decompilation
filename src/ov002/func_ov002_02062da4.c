#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_02062da4(u32 a0, u32 a1)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u32 *)((u8 *)(v0) + 8) = a0;
    *(u8 *)((u8 *)(v0) + 12) = a1;
    return a0;
}
