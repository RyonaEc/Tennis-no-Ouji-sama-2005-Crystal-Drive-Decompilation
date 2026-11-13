#include "types.h"

s32 func_02036808(u8 *p)
{
    s32 n = -1;
    do {
        n++;
    } while (*p++ != 0);
    return n;
}
