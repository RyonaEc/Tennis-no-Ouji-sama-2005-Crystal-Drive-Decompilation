#include "types.h"

u8 *func_02034450(u8 *p, u32 val, u32 n)
{
    u32 c = val & 0xff;
    if (n != 0) {
        do {
            u32 v = *p++;
            if (v == c) return p - 1;
        } while (--n);
    }
    return 0;
}
