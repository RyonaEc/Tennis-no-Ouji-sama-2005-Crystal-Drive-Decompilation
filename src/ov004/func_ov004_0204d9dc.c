#include "types.h"

void func_ov004_0204d9dc(u8 *p)
{
    if (*(u32 *)(p + 0x10) == 0)
        return;
    *(u32 *)p = 0;
    *(u16 *)(p + 8) = 2;
    *(u16 *)(p + 0xa) = 0;
    *(u16 *)(p + 0xc) = 0;
    *(u16 *)(p + 0x20) = 0;
    *(u16 *)(p + 0x22) = 0;
}