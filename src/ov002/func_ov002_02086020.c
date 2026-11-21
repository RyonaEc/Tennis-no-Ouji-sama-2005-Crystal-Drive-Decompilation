#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_02086020(u32 mask)
{
    u8 *p = data_ov002_02102070 + 0x47d4;

    *p |= mask;
}