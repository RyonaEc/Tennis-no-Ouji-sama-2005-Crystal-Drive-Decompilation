#include "types.h"

void func_0202da4c(u32 a0, u32 a1)
{
    while (*(volatile u32 *)0x040001a4 & 0x80000000) {
    }
    *(volatile u8 *)0x040001a1 = 0xc0;
    *(volatile u8 *)0x040001a8 = a0 >> 24;
    *(volatile u8 *)0x040001a9 = a0 >> 16;
    *(volatile u8 *)0x040001aa = a0 >> 8;
    *(volatile u8 *)0x040001ab = a0;
    *(volatile u8 *)0x040001ac = a1 >> 24;
    *(volatile u8 *)0x040001ad = a1 >> 16;
    *(volatile u8 *)0x040001ae = a1 >> 8;
    *(volatile u8 *)0x040001af = a1;
}
