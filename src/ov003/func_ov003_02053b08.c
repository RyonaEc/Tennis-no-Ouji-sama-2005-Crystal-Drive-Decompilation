#include "types.h"

extern u32 data_ov003_020eb720;

u32 func_ov003_02053b08(u32 i)
{
    u32 base = data_ov003_020eb720;
    u8 idx = i & 3;

    return (u32)&((u16 *)(base + 0x2e4))[idx * 12];
}