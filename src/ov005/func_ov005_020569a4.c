#include "types.h"

extern u8 data_ov005_020798e0[];

u32 func_ov005_020569a4(u32 a0)
{
    u32 v0;
    v0 = *(u32 *)((u8 *)(((u32)data_ov005_020798e0 + (a0 << 0x2))));
    *(u32 *)((u8 *)(0x4000484)) = v0;
    return v0;
}
