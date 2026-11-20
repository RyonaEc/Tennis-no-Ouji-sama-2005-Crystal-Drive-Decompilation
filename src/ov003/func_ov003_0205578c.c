#include "types.h"

s32 func_ov003_0205578c(u32 a0, u32 *a1)
{
    u32 acc;
    u16 i;
    s32 n;

    acc = 0x2000;
    if (a0 >= 0x20)
        goto end;
    n = a0 + 1;
    i = 0;
    if (n <= 0)
        goto end;
    do {
        if (i != 0)
            acc += a1[i - 1];
        i = i + 1;
    } while (i < n);
end:
    return acc;
}
