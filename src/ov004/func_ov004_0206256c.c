#include "types.h"

extern u8 *func_0200faa8(void);

u32 func_ov004_0206256c(s32 a0)
{
    u8 *p = func_0200faa8();

    if (!p)
        return 0;
    return p[a0 / 8] & (1 << (a0 % 8));
}