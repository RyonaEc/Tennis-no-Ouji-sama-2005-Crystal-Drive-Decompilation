#include "types.h"

s32 func_ov002_0207b864(s32 a0)
{
    if (a0 > 0x3c) return 0x3c;
    if (a0 < -0x3c) a0 = -0x3c;
    return a0;
}
