#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0206d5e0(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    *(u32 *)(b + 0x1dd20c) = 0;
}
