#include "types.h"

extern u8 *data_0204b6ec;

void func_0202e418(u32 i, u32 v)
{
    u8 *b = data_0204b6ec;

    *(u32 *)(b + i * 4 + 0x14) = v;
}