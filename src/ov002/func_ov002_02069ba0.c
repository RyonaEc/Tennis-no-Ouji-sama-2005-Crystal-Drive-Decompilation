#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02069ba0(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    *(u32 *)(b + 0x1ddbfc) = 0;
}
