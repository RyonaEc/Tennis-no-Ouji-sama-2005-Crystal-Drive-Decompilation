#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32 enabled);

void func_020266c0(u32 dmaNo)
{
    u32 enabled = func_02026350();
    volatile u16 *dmaCntp = &((volatile u16 *)0x040000b0)[dmaNo * 6 + 5];

    *dmaCntp &= ~0x3a00;
    *dmaCntp &= ~0x8000;

    { s32 dummy = dmaCntp[0]; }
    { s32 dummy = dmaCntp[0]; }

    func_02026364(enabled);
}