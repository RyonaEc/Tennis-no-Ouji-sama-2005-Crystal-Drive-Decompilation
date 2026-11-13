#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0206e474(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)(((v0 + 0x1e6c) + a0)) + 140) = 0x1;
    *(u8 *)((u8 *)(((v0 + 0x1e6c) + a0)) + 144) = 0x0;
    *(u8 *)((u8 *)(((v0 + 0x1e6c) + a0)) + 148) = 0x0;
    return 0x0;
}
