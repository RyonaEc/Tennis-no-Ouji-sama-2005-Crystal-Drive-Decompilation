#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_02084210(void)
{
    u8 *p = data_ov002_02102070 + 0x36ac;

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
}