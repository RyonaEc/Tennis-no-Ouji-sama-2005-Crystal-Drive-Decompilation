#include "types.h"

extern void func_02022b2c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5,
                          u32 a6, u32 a7, u32 a8);

void func_02018c70(void)
{
    *(volatile u32 *)0x04000580 = 0xbfff0000;
    func_02022b2c(0, 0xc0000, 0, 0x100000, 0xffc00000, 0x400000, 0x400000, 1, 0);
    *(volatile u32 *)0x0400044c = 0;
}