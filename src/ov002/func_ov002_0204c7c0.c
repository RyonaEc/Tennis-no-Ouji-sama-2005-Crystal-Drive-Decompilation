#include "types.h"

extern u8 data_ov002_02102070[];

u32 func_ov002_0204c7c0(u32 a0, u32 a1)
{
    u32 v0;
    v0 = *(u32 *)(data_ov002_02102070);
    *(u8 *)((u8 *)((a0 * 0x1c + (v0 + 0x274))) + 17) = a1;
    *(u8 *)((u8 *)((a0 * 0x1c + (v0 + 0x274))) + 18) = 0x0;
    return a0;
}
