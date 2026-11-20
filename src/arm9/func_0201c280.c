#include "types.h"

extern u8 data_02047608[];
extern void *func_02017784(u8 *, u32);
extern void func_0201779c(u8 *, void *);

u32 func_0201c280(u32 a0, u32 a1)
{
    u8 *s = data_02047608 + a0 * 0x1c;
    u8 *p = func_02017784(s + 0xc, 0);

    if (p == 0) return 0;
    func_0201779c(s + 0xc, p);
    *(u32 *)(p + 0xc) = a1;
    *(u32 *)(a1 + 8) = (u32)p;
    return *(u32 *)(p + 8);
}