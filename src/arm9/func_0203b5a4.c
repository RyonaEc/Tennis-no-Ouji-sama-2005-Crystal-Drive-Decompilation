#include "types.h"

extern u32 func_0203b564(u32);

s32 func_0203b5a4(u32 a0, u8 *a1, u32 *a2)
{
    u32 i;
    u32 n = *a2;

    for (i = 0; i < n; i++)
        func_0203b564((u32)(a1 + i));
    return 0;
}