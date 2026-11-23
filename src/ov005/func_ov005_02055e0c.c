#include "types.h"

extern u8 data_ov005_020c9740[];
extern u32 func_ov005_02051aac(void);
extern u32 func_ov005_02051c64(u32, u32 *);
extern u32 func_ov005_02051c1c(u32, u32 *);
extern s32 func_ov005_02051ad4(s32);
extern u32 func_ov005_020569a4(u32);

s32 func_ov005_02055e0c(u32 a0, u32 a1, u8 *a2)
{
    u8 *base = *(u8 **)data_ov005_020c9740;
    u32 x, y, e, c, w;

    if (a2 == 0)
        return 0;
    func_ov005_02051aac();
    x = func_ov005_02051c64(*(u16 *)(a2 + 4), (u32 *)(base + 0x734c));
    y = func_ov005_02051c1c(*(u16 *)(a2 + 4), (u32 *)(base + 0x754c));
    e = func_ov005_02051ad4(0x10);
    c = func_ov005_02051ad4(8);
    w = (x >> 3) | 0x4c000000;
    w = w | (c << 20);
    w = w | (e << 23);
    *(volatile u32 *)0x040004a8 = w | 0x20000000;
    *(volatile u32 *)0x040004ac = y >> 4;
    *(volatile u32 *)0x040004a4 = (*(u16 *)(a2 + 0x1c) << 16) | 0xc0;
    *(volatile u32 *)0x04000500 = 1;
    *(volatile u32 *)0x04000488 = 0x800000;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0x1000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = 0;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0x10001000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = 0x70;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0x10000000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = 0x00800070;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000504 = 0;
    return 1;
}