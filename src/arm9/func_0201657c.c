#include "types.h"

extern void func_020176f0(u16);

void func_0201657c(u8 *p)
{
    if (*(u16 *)(p + 2) != 0)
        func_020176f0(*(u16 *)(p + 2));
}
