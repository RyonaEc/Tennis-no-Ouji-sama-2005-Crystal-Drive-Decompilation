#include "types.h"

extern u32 func_02018018(u32 a0, u32 a1, u32 a2);

u32 func_02017bb0(u32 a0, u32 a1, u32 a2)
{
    u32 end = (a1 + a0) & ~3;
    u32 start = (a0 + 3) & ~3;

    if (start > end || end - start < 0x4c)
        return 0;
    return func_02018018(start, end, a2);
}