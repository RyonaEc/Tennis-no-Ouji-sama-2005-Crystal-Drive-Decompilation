#include "types.h"

extern u8 *func_0200faa8(void);
s32 func_ov004_0206248c(void)
{
    u8 *p = func_0200faa8();

    if (!p)
        return 0;
    return p[0xf] + 0x309;
}
