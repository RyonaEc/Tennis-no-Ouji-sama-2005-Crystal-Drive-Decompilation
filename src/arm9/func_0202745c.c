#include "types.h"

extern u8 data_02049030[];
s32 func_0202745c(void)
{
    u8 *p = *(u8 **)data_02049030;
    s32 n = 0;
    while (p != 0) {
        p = *(u8 **)p;
        n++;
    }
    return n;
}
