#include "types.h"

s32 func_02017af8(u8 *p)
{
    u8 *node = *(u8 **)(p + 0x24);
    s32 sum = 0;

    while (node != 0) {
        sum += *(s32 *)(node + 4);
        node = *(u8 **)(node + 0xc);
    }
    return sum;
}
