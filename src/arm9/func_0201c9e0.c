#include "types.h"

extern u8 data_02047dcc[];
extern void func_0201be3c(u32 a0);
extern void func_0201779c(u8 *a0, u8 *a1);

void func_0201c9e0(u8 *a0)
{
    u32 *q = (u32 *)(a0 + 0x24);

    func_0201be3c(*(u32 *)(a0 + 0x40));
    func_0201779c(data_02047dcc, a0);
    *q &= ~1;
}