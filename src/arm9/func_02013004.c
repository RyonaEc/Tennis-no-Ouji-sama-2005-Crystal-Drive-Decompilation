#include "types.h"

extern u8 data_02042a68[];
extern void func_02026b88(u32, u32, u32);

void func_02013004(u32 a0, u32 a1)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x14) + 0x104);
    *(u8 *)(p + 1) = a0;
    if (*(u8 *)(p + 1) != 1) return;
    func_02026b88(a1, (u32)(p + 2), 8);
}