#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32);
extern void func_01ff844c(u32, u32 *, u32, u32);

void func_02026934(u32 ch, u32 src, u32 dst, u32 flags)
{
    volatile u32 *reg;
    u32 *fill;
    u32 h;

    if (flags == 0)
        return;
    reg = (volatile u32 *)(0x040000b0 + (ch * 3 + 2) * 4);
    while (*reg & 0x80000000)
        ;
    h = func_02026350();
    fill = (u32 *)(0x040000e0 + ch * 4);
    *fill = dst;
    func_01ff844c(ch, fill, src, (flags >> 2) | 0x85000000);
    func_02026364(h);
    while (*reg & 0x80000000)
        ;
}