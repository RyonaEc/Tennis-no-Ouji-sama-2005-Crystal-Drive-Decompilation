#include "types.h"

extern s32 func_02027e98(s32, s32, s32);

void func_02027340(void)
{
    s32 a = 7;
    s32 b = 0;

    while (func_02027e98(a, b, b) < 0)
        ;
}