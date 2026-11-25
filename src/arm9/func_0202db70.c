#include "types.h"

extern u32 data_0204aec0[];
extern void func_02026b88(register const void *srcp, register void *dstp, register u32 size);

u32 func_0202db70(u8 *a0)
{
    u32 *g = data_0204aec0;
    u32 m = g[6] & -0x200;
    u32 off;
    u32 n;

    if (m == *(u32 *)(a0 + 8)) {
        off = g[6] - m;
        n = 0x200 - off;
        if (n > g[8])
            n = g[8];
        func_02026b88((void *)(a0 + 0x20 + off), (void *)g[7], n);
        g[6] += n;
        g[7] += n;
        g[8] -= n;
    }
    return g[8] != 0;
}