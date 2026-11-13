#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_0206e43c(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    s32 i;
    for (i = 0; i < 4; i++)
        *(u8 *)(p + i + 0x8c) = 0;
}
