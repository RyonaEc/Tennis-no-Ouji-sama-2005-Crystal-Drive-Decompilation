#include "types.h"

extern void func_020183ac(u8 *, s32, s32);
extern void func_02018330(u8 *, s32, s32);

void func_02018248(u8 *p, s32 n, s32 count)
{
    u8 *q = p + 0x24;

    if (n == 0)
        n = 1;
    n = (n + 3) & ~3;
    if (count >= 0)
        func_020183ac(q, n, count);
    else
        func_02018330(q, n, -count);
}