#include "types.h"

extern s32 func_02010150(s32);

s32 func_02008bec(void)
{
    s32 i;
    s32 n = 0;

    for (i = 0; i < 0x3c; i++) {
        if (func_02010150(i))
            n++;
    }
    return n;
}
