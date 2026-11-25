#include "types.h"

extern void func_02022554(u32 enable, u32 fogMode, u32 fogSlope, s32 fogOffset);
extern void func_0202250c(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

void func_ov008_0204c9b8(void)
{
    u16 v;

    v = *(u16 *)0x04000060;
    *(u16 *)0x04000060 = (v & 0xffffcffd) | 2;
    v = *(u16 *)0x04000060;
    *(u16 *)0x04000060 = (v & ~0x3000) | 0x10;
    *(u16 *)0x04000060 &= 0xcffb;
    v = *(u16 *)0x04000060;
    *(u16 *)0x04000060 = (v & ~0x3000) | 8;
    v = *(u16 *)0x04000060;
    *(u16 *)0x04000060 = v & 0x0000cfdf;
    func_02022554(0, 0, 0, 0);
    func_0202250c(0, 0, 0x7fff, 0x3f, 0);
    *(u32 *)0x04000580 = 0xbfff0000;
}