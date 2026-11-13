#include "types.h"

s32 func_02034404(u32 a0, u8 a1)
{
    if (a0 == 0) return 0;
    *(u8 *)a0 = a1;
    return 1;
}
