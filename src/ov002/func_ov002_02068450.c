#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02068450(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    *(u8 *)(p + 0x1000 + 0x7d4) = 0;
    *(u8 *)(p + 0x1000 + 0x7d5) = 0;
    *(u16 *)(p + 0x1700 + 0xd6) = 0xa;
}
