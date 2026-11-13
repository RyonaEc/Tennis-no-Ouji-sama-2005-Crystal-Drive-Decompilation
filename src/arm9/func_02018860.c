#include "types.h"

void func_02018860(u8 *p)
{
    *(volatile u16 *)(p + 0xa) = 0;
    *(u16 *)(p + 8) = *(volatile u16 *)(p + 0xa);
    *(u16 *)(p + 0xc) = 0;
    *(u32 *)(p + 0x10) = 0;
}
