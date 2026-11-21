#include "types.h"

extern void func_02028af4(u8 *a0, u32 a1);

void func_020289f4(u8 *a0, u32 a1)
{
    *(u32 *)(a0 + 0xc) |= 4;
    *(u32 *)(a0 + 0x2c) = *(u32 *)(a0 + 8);
    *(u32 *)(a0 + 0x34) = 0;
    *(u16 *)(a0 + 0x32) = 0;
    *(u16 *)(a0 + 0x30) = a1;
    func_02028af4(a0, 2);
}