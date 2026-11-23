#include "types.h"

extern u8 data_02048c74[];
extern void func_02021538(u32);

void func_0202093c(u32 sel)
{
    u16 old1;
    u16 old0;

    old1 = *(u16 *)(data_02048c74 + 0x16);
    old0 = *(u16 *)(data_02048c74 + 0);
    *(u16 *)(data_02048c74 + 0) = (old0 | old1) & ~sel;
    *(u16 *)(data_02048c74 + 0x16) = sel;
    if (sel != 0) {
        if (sel == 0x80) {
            *(volatile u32 *)0x04001000 |= 0x40000000;
            *(volatile u8 *)0x04000248 = 0x82;
        }
    } else {
        *(volatile u32 *)0x04001000 &= ~0x40000000;
    }
    func_02021538(*(u16 *)(data_02048c74 + 0));
}