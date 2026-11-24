#include "types.h"

extern s32 func_0202bfdc(u32 addr, u16 *reg);

s32 func_0202bda0(s32 *top, s32 *bottom)
{
    u16 reg;
    s32 result;

    result = func_0202bfdc(0, &reg);
    if (result != 0)
        return result;

    if (top)
        *top = (reg & 0x8) ? 1 : 0;

    if (bottom)
        *bottom = (reg & 0x4) ? 1 : 0;

    return result;
}