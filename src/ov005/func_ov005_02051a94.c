#include "types.h"

s32 func_ov005_02051a94(s32 a, s32 b)
{
    if (a == 0)
        return (b << 5) - 0x1000;
    return 0x1000 - (b << 5);
}
