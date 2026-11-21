#include "types.h"

extern u32 data_ov003_020eb720;

u16 *func_ov003_02053b30(void)
{
    u8 *b = (u8 *)data_ov003_020eb720;

    return (u16 *)(b + 0x2e4) + *(u16 *)(b + 0x26c) * 12;
}
