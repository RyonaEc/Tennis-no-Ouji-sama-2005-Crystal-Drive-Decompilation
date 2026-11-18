#include "types.h"

s32 func_02034418(u16 *dst, u8 *src, s32 flag)
{
    if (src == 0)
        return 0;
    if (flag == 0)
        return -1;
    if (dst != 0)
        *dst = *src;
    if (*src == 0)
        return 0;
    return 1;
}