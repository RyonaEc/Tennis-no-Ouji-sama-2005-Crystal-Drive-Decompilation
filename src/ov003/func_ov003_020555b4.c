#include "types.h"

s32 func_ov003_020555b4(s32 a0, s32 a1)
{
    if (a0 == 0) return (a1 << 5) - 0x1000;
    else return 0x1000 - (a1 << 5);
}
