#include "types.h"

u16 *func_ov003_02053b30(void)
{
    u8 *b = (u8 *)*(u32 *)0x020eb720;

    return (u16 *)(b + 0x2e4) + *(u16 *)(b + 0x26c) * 12;
}
