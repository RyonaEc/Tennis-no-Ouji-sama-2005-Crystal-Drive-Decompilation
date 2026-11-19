#include "types.h"

extern void func_0201ca18(u32 *p);

void func_0201cb54(u32 *p)
{
    s32 f = (s32)(p[9] << 31) >> 31;

    if (f == 0)
        return;
    func_0201ca18(p);
}