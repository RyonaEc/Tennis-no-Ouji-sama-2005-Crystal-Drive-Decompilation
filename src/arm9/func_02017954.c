#include "types.h"

void func_02017954(u8 *a0, u8 *a1)
{
    u16 off = *(u16 *)(a0 + 0xa);
    u16 *cnt = (u16 *)(a0 + 8);

    *(u32 *)(a1 + off + 4) = 0;
    *(u32 *)(a1 + off) = 0;
    *(u32 *)a0 = (u32)a1;
    *(u32 *)(a0 + 4) = (u32)a1;
    *cnt += 1;
}

