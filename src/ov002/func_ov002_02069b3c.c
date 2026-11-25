#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fc558[];
extern u32 func_ov002_0204f5a0(u32, u32, u32);

void func_ov002_02069b3c(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    u8 *p = g + 0x1e6c;

    *(u32 *)p |= 0x400;
    *(u8 *)(p + 0x5e0) = 0;
    *(u32 *)(p + 0x628) = 0;
    *(u32 *)(p + 0x62c) = 0x238e3;
    func_ov002_0204f5a0((u32)data_ov002_020fc558, 0x5000000, 0x20);
}