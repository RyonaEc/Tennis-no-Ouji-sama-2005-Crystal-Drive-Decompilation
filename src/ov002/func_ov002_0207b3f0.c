#include "types.h"

extern u32 func_ov002_0204c824(u32, u32, u32);
extern u8 data_ov002_02102070[];

u32 func_ov002_0207b3f0(u32 a0)
{
    u32 t0;
    t0 = func_ov002_0204c824(*(u8 *)((u8 *)((a0 * 0x158 + (*(u32 *)(data_ov002_02102070) + 0x2e4))) + 2), *(u8 *)((u8 *)((a0 * 0x158 + (*(u32 *)(data_ov002_02102070) + 0x2e4))) + 1), (*(u32 *)(data_ov002_02102070) + 0x2e4));
    return t0;
}
