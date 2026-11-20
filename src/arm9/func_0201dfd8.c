#include "types.h"

extern u32 *func_0201d4f0(u32);
extern s32 func_0201ddd0(u32, u32);

s32 func_0201dfd8(u32 a0, u32 a1)
{
    u32 *p = func_0201d4f0(a0);

    if (!p)
        return 0;
    return func_0201ddd0(*p, a1) ? 1 : 0;
}