#include "types.h"

extern u32 func_02019620(u32, u32);
u32 func_020195dc(u32 a0, u32 a1)
{
    return func_02019620(a0, a1) + 0x100 + (a1 << 3);
}
