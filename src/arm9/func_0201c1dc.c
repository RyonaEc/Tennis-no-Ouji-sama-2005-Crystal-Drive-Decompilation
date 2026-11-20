#include "types.h"

extern u32 func_02017784(u32, u32);
extern void func_0201780c(u32, u32, u8 *);

void func_0201c1dc(u32 a0, u8 *a1)
{
    u32 n = func_02017784(a0, 0);

    while (n != 0)
    {
        if (a1[0x3d] < *(u8 *)(n + 0x3d)) break;
        n = func_02017784(a0, n);
    }
    func_0201780c(a0, n, a1);
    *(u32 *)(a1 + 4) = a0;
}