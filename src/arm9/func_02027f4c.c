#include "types.h"

s32 func_02027f4c(s32 bit, s32 i)
{
    u8 *base = (u8 *)0x027ffc00;

    return (*(u32 *)(base + 0x388 + i * 4) & (1 << bit)) != 0;
}
