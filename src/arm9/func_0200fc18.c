#include "types.h"

extern u8 data_02042a68;

void func_0200fc18(s32 d)
{
    u8 *q;

    *(u32 *)(*(u32 *)(*(u32 *)((u8 *)&data_02042a68 + 0x10) + 0x154) + 0x10) += d;
    q = (u8 *)(*(u32 *)(*(u32 *)((u8 *)&data_02042a68 + 0x10) + 0x154));
    if (*(u32 *)(q + 0x10) > 0x80bbb040)
        *(u32 *)(q + 0x10) = 0x80bbb040;
}