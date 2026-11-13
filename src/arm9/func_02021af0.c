#include "types.h"

void func_02021af0(u8 *a0, u16 a1, s32 a2)
{
    if (a2 < 0)
    {
        *(u16 *)a0 = a1 | 0xc0;
        *(u16 *)(a0 + 4) = -a2;
    }
    else
    {
        *(u16 *)a0 = a1 | 0x80;
        *(u16 *)(a0 + 4) = a2;
    }
}
