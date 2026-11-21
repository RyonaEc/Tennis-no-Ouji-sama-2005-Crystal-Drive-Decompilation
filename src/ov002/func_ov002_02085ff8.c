#include "types.h"

extern u8 *data_ov002_02102070;

void func_ov002_02085ff8(u32 a0)
{
    u8 *p = data_ov002_02102070 + 0x47d4;

    *p = *p & ~a0;
}