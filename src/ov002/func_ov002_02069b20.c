#include "types.h"

extern u32 func_ov002_0204f5a0(u32, u32, u32);
extern u8 data_ov002_020fc538[];

u32 func_ov002_02069b20(void)
{
    u32 t0 = func_ov002_0204f5a0((u32)data_ov002_020fc538, 0x5000000, 0x20);
    return t0;
}
