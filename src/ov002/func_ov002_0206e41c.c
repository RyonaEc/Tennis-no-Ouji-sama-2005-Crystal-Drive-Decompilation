#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0206e41c(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)(((v0 + a0) + 0x1000)) + 3832) = 0x0;
    return ((v0 + a0) + 0x1000);
}
