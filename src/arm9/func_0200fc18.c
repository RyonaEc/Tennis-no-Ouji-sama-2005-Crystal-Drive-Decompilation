#include "types.h"

extern u8 data_02042a68[];

void func_0200fc18(u32 v)
{
    ((u32 *)*(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x154))[4] += v;
    if (((u32 *)*(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x154))[4] > 0x80bbb040u)
        ((u32 *)*(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x154))[4] = 0x80bbb040u;
}