#include "types.h"

extern u8 *data_02047f00;

void func_0201d1c4(u32 i, u32 v)
{
    u8 *b = data_02047f00;
    u8 *t = *(u8 **)(b + 0x7c);

    *(u32 *)(t + i * 0x10 + 0x14) = v;
}