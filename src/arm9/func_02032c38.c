#include "types.h"

u32 func_02032c38(u8 *s)
{
    u32 n = 0;

    while (((u32)s & 1) ? (((*(u16 *)(s - 1)) & 0xff00) >> 8)
                        : ((*(u16 *)s) & 0xff)) {
        n++;
        s++;
    }
    return n;
}