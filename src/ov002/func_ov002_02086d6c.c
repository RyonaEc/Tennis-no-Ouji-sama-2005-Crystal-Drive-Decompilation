#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02086d6c(void)
{
    u8 *p = (u8 *)(*(u8 **)data_ov002_02102070 + 0x47d0);
    p[1] = 1;
    p[2] = 0;
    p[3] = 0;
    p[5] = 0;
}
