#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02068c24(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x1e6c);
    p[0x680] = 0;
    p[0x681] = 0;
}
