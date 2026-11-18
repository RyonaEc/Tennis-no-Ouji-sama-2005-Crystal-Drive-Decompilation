#include "types.h"

extern u32 data_02049034;

s32 func_02027488(u32 a0)
{
    u32 t = data_02049034;

    if (a0 > t)
        return (a0 - t) >= 0x80000000;
    return (t - a0) < 0x80000000;
}
