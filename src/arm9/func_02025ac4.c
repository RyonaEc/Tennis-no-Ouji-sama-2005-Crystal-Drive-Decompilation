#include "types.h"

extern u32 func_020259e0(u8 *);
extern void func_0202514c(u32);

void func_02025ac4(u8 *a0)
{
    u32 p;

    if (*(u32 *)(a0 + 0x7c) == 0) return;
    do
    {
        p = func_020259e0(a0);
        *(u32 *)(p + 8) = 0;
        *(u32 *)(p + 4) = 0;
        func_0202514c(p);
    } while (*(u32 *)(a0 + 0x7c) != 0);
}