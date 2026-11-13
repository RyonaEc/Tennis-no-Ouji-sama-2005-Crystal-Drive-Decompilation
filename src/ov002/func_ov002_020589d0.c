#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_020589d0(u32 a0)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *p = b + a0 * 0x158 + 0x400;
    *(u16 *)(p + 0x1c) = 0x20;
}
