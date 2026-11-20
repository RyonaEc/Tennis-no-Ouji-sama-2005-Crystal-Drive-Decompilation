#include "types.h"

u32 func_020063f8(u8 *p, s32 n)
{
    s32 i;
    u16 sum = 0;
    s32 m = (s32)(u16)(n - 2);

    for (i = 0; i < m; i++) {
        sum += *p;
        p++;
    }
    return (u16)(~sum - m);
}