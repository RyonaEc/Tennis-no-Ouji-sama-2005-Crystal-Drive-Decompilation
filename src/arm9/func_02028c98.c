#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_02029d48(u8 *a0);
extern void func_0202514c(u8 *a0);

void func_02028c98(u8 *a0, u32 a1)
{
    u32 h = func_02026350();

    func_02029d48(a0);
    *(u32 *)(a0 + 0xc) &= ~0xf;
    *(u32 *)(a0 + 0x14) = a1;
    func_0202514c(a0 + 0x18);
    func_02026364(h);
}