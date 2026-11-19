#include "types.h"

void func_0202230c(u8 *p, u32 a1, u32 a2)
{
    *(u32 *)(p + 0x10) = 0;
    *(u8 *)(*(u32 *)p) = a1;
    *(u32 *)(*(u32 *)(p + 4)) = a2;
}