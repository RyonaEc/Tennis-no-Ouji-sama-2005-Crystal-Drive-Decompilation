#include "types.h"

extern u8 data_02041eb0[];
u8 *func_0202d080(u32 k)
{
    u8 *e = data_02041eb0;
    while (1) {
        u32 v = *(u32 *)e;
        if (v == k) return e;
        if (v == 0) return 0;
        e += 0x28;
    }
}
