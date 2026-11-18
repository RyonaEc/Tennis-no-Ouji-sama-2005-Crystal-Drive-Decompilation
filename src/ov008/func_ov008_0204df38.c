#include "types.h"

void func_ov008_0204df38(void)
{
    u8 *p = (u8 *)*(u32 *)0x0204f440;

    *(u16 *)(p + 0x36) = 0x1a03;
    *(u16 *)(p + 0x38) = 0x84;
}
