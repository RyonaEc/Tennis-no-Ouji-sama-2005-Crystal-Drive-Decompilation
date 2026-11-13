#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_020590e8(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *p = b + 0x1824;
    if ((*(u32 *)(p + 0xbc) & 0x30000000) == 0)
        *(u8 *)(b + *(u8 *)(p + 0x8f) + 0x13c) = 1;
}
