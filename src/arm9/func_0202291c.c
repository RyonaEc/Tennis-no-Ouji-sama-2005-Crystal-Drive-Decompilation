#include "types.h"

void func_0202291c(u32 a0, u32 a1)
{
    volatile u32 *r = (volatile u32 *)0x04000468;
    *r = a1;
    *r = a0;
    *r = 0;
    *r = -a0;
    *r = a1;
    *r = 0;
    *r = 0;
    *r = 0;
    *r = 0x1000;
}
