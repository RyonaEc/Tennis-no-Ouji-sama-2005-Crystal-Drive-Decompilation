#include "types.h"

void func_0201fca8(u8 *p)
{
    *(volatile u32 *)(p + 4) = 0;
    *(u32 *)p = *(volatile u32 *)(p + 4);
    *(volatile u32 *)(p + 0xc) = 0;
    *(u32 *)(p + 8) = *(volatile u32 *)(p + 0xc);
}
