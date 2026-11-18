#include "types.h"

s32 func_ov002_02078858(s32 a0, s32 a1, s32 a2, s32 a3)
{
    s32 dx = a2 - a0;
    s32 dy = a3 - a1;

    if (func_02034148(dy) > func_02034148(dx) * 2) {
        if (dy < 0)
            return 4;
        return 3;
    }
    if (dx < 0)
        return 1;
    return 2;
}
