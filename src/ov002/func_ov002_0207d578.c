#include "types.h"

extern u8 data_ov002_020fcea0[];
extern u8 data_ov002_02102070[];

u32 func_ov002_0207d578(u32 a0)
{
    u32 v0, v2;
    v0 = *(u32 *)((u8 *)(((u32)data_ov002_020fcea0 + (*(u8 *)((u8 *)((a0 * 0x158 + *(u32 *)(data_ov002_02102070))) + 740) << 0x3))));
    v2 = *(u32 *)(data_ov002_02102070);
    return ((u32 (*)(u32, u32, u32, u32))v0)(a0, v0, (u32)data_ov002_020fcea0, v2);
}
