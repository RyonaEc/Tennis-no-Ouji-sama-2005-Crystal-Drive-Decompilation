#include "types.h"

extern u8 data_ov008_0204f440[];

void func_ov008_0204d940(void)
{
    u8 *p = *(u8 **)data_ov008_0204f440;

    *(u8 *)(p + 0x22) = 0;
    *(u32 *)(p + 0x28) = 0x1000;
    *(u8 *)(p + 0x24) = 0;
    *(u32 *)(p + 0x2c) = 0;
    *(u8 *)(p + 0x23) = 0;
    *(u8 *)(p + 0x25) = 0;
}