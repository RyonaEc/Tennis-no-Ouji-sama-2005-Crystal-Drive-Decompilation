#include "types.h"

void func_0201d1c4(u32 i, u32 v)
{
    u8 *b = *(u8 **)0x02047f00;
    u8 *t = *(u8 **)(b + 0x7c);

    *(u32 *)(t + i * 0x10 + 0x14) = v;
}