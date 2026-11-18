#include "types.h"

extern u32 data_020475e8;

s32 func_0201be5c(void)
{
    s32 i;
    u32 mask = 1;

    for (i = 0; i < 8; i++) {
        if (!(data_020475e8 & mask)) {
            data_020475e8 |= mask;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}
