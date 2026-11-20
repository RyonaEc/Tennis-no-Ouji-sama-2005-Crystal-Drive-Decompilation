#include "types.h"

extern u8 data_02042a68[];
extern u32 func_02012580(u32);
extern void func_02026a50(u32, u32, u32);

void func_02012534(u8 *a0)
{
    u8 *p = *(u8 **)(data_02042a68 + 0x1c);
    if (p == 0) return;
    func_02026a50((u32)(p + (func_02012580(*(u16 *)(a0 + 4)) << 7)),
                  *(u32 *)a0, 0x80);
}