#include "types.h"

extern u8 data_02042a68[];
extern u32 func_02014670(u32);
extern void func_020146c8(u32);
extern u8 *func_02014d34(u32);
u32 func_02013650(void)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x14) + 0x104);
    u8 *q;
    if (func_02014670(8) == 0)
    {
        *(u8 *)p = 8;
        func_020146c8(0);
        return 0;
    }
    q = func_02014d34(0);
    if (*(u8 *)(q + 1) == 1) return *(u8 *)(q + 2);
    return 0;
}
