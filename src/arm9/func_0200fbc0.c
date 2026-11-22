#include "types.h"

extern u8 data_02042a68;

void func_0200fbc0(s32 d)
{
    u8 *q;

    *(u16 *)(*(u32 *)(*(u32 *)((u8 *)&data_02042a68 + 0x10) + 0x154) + 2) += d;
    q = (u8 *)(*(u32 *)(*(u32 *)((u8 *)&data_02042a68 + 0x10) + 0x154));
    if (*(u16 *)(q + 2) > 50000)
        *(u16 *)(q + 2) = 50000;
}