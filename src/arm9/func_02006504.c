#include "types.h"

extern u8 data_02042a68[];
extern void func_0201e5f8(u8 *, u32, u32, u32, u32);
extern void func_0201e53c(u8 *, u32, u32, u32, u32, u32);

void func_02006504(u32 a0, s32 a1)
{
    u8 *ip = *(u8 **)(data_02042a68 + 8);

    if (a1 >= 0x2f2)
        func_0201e5f8(ip + 0x94, 5, 0xffffffff, 0xffffffff, a1);
    else
        func_0201e53c(ip + 0x90, a0 ? 4 : 1, *(u32 *)(ip + 0xa8), 0xffffffff, *(u32 *)(ip + 0xa4), a1);
}