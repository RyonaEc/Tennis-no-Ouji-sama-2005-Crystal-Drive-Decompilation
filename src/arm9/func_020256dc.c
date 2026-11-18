#include "types.h"

s32 func_020256dc(void)
{
    u16 *p;

    if (*(u16 *)0x027ffc10 == 0)
        p = (u16 *)0x027ff814;
    else
        p = (u16 *)0x027ffc14;
    return *p == 1;
}