#include "types.h"

s32 func_02033170(u32 v)
{
    u8 *base = (u8 *)0x027fff60;

    return (v & *(u32 *)(base + 0x14)) != 0;
}
