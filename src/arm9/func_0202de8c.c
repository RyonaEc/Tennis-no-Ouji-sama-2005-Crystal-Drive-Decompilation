#include "types.h"

void func_0202de8c(void)
{
    u16 *p = (u16 *)0x027fff96;

    if (*p & 1)
        *p &= ~1;
}