#include "types.h"

extern u32 data_ov008_0204f440;

void func_ov008_0204df38(void)
{
    u8 *p = (u8 *)data_ov008_0204f440;

    *(u16 *)(p + 0x36) = 0x1a03;
    *(u16 *)(p + 0x38) = 0x84;
}
