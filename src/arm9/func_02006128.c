#include "types.h"

extern u8 data_02042a68[];
extern void func_02026a04(u32, u32, u32);
void func_02006128(void)
{
    volatile u32 v = 0;
    func_02026a04(v, *(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x15c), 0x400);
}
