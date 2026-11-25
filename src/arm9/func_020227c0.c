#include "types.h"

extern void func_02022704(void);
extern void func_02022654(void);
extern void func_02022454(void);

void func_020227c0(void)
{
    func_02022704();

    *(volatile u32 *)0x04000504 = 0;

    while (*(volatile u32 *)0x04000600 & 0x08000000)
        ;

    *(volatile u16 *)0x04000060 = 0;
    *(volatile u32 *)0x04000600 = 0;
    *(volatile u32 *)0x04000010 = 0;

    *(volatile u16 *)0x04000060 |= 0x2000;
    *(volatile u16 *)0x04000060 |= 0x1000;
    *(volatile u16 *)0x04000060 = *(volatile u16 *)0x04000060 & 0xffffcffd;
    *(volatile u16 *)0x04000060 = (*(volatile u16 *)0x04000060 & 0xffffcfff) | 0x10;
    *(volatile u16 *)0x04000060 = *(volatile u16 *)0x04000060 & 0x0000cffb;

    *(volatile u32 *)0x04000600 |= 0x8000;
    *(volatile u32 *)0x04000600 = (*(volatile u32 *)0x04000600 & 0x3fffffff) | 0x80000000;

    func_02022654();

    *(volatile u32 *)0x04000350 = 0;
    *(volatile u16 *)0x04000354 = 0x7fff;
    *(volatile u16 *)0x04000356 = 0;
    *(volatile u32 *)0x04000358 = 0;
    *(volatile u16 *)0x0400035c = 0;

    *(volatile u16 *)0x04000008 = *(volatile u16 *)0x04000008 & ~3;

    func_02022454();

    *(volatile u32 *)0x040004a4 = 0x001f0080;
    *(volatile u32 *)0x040004a8 = 0;
    *(volatile u32 *)0x040004ac = 0;
}