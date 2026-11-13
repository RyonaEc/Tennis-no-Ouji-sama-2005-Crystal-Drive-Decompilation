#include "types.h"

void func_0201abd0(u8 *p)
{
    *(u32 *)p = 0;
    *(u32 *)(p + 4) = 2;
    *(volatile u32 *)(p + 8) = 0;
    *(volatile u32 *)(p + 0xc) = 0;
    *(u32 *)(p + 0x18) = 0;
    *(u32 *)(p + 0x20) = 1;
    *(u32 *)(p + 0x1c) = 0;
}
