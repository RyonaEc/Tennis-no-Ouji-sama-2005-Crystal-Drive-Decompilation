#include "types.h"

extern u8 data_02049038[];
s32 func_02027430(void)
{
    u8 *p = *(u8 **)data_02049038;
    s32 n = 0;
    while (p != 0) {
        p = *(u8 **)p;
        n++;
    }
    return n;
}
