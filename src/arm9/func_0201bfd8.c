#include "types.h"

extern u8 data_02047608[];
extern void func_0201db4c(u32);
extern void func_0201779c(u32, u32);

void func_0201bfd8(u8 *a0)
{
    u32 n = *(u32 *)(a0 + 8);
    u32 *prev;

    if (n == 0) return;
    func_0201db4c(n);
    prev = (u32 *)(a0 + 0xc);
    if (*prev != 0)
    {
        *(u32 *)(*prev + 8) = 0;
        return;
    }
    func_0201779c((u32)(data_02047608 + *(u32 *)(a0 + 0x10) * 0x1c + 0xc), (u32)a0);
}