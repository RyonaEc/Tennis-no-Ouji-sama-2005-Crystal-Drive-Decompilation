#include "types.h"

u8 *func_02034494(u8 *d, u8 *s, u32 n)
{
    u8 *p = d;
    if (n == 0) return d;
    do {
        *p++ = *s++;
    } while (--n);
    return d;
}
