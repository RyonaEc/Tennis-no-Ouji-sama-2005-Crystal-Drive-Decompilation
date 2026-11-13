#include "types.h"

extern void func_02027278(u8);

void func_0201c470(u8 **pp)
{
    u8 *p = *pp;

    if (p != 0)
        func_02027278(p[0x3c]);
}
