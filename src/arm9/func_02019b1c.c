#include "types.h"

void func_02019b1c(u8 *p, u8 *n)
{
    *(u32 *)(n + 0x34) = *(u32 *)(p + 0x30);
    *(u32 *)(p + 0x30) = (u32)n;
}
