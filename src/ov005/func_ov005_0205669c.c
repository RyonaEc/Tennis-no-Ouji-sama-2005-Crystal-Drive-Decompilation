#include "types.h"

typedef struct {
    u32 a, b, c, d;
} Quad;

extern u8 data_ov005_020c9740[];
extern u32 data_ov005_020c9694[];
extern u32 func_ov005_02051aac(void);
extern s32 func_ov005_02051c64(u32, u32 *);
extern u32 func_ov005_02051c1c(u32, u32 *);
extern u32 func_ov005_02051ad4(u32);
extern void func_ov005_020569a4(u32);

s32 func_ov005_0205669c(u32 a0, u32 a1, u8 *a2)
{
    u8 *base = *(u8 **)data_ov005_020c9740;
    u32 x, y, e, c, w;
    Quad v;

    func_ov005_02051aac();
    x = func_ov005_02051c64(*(u16 *)(a2 + 4), (u32 *)(base + 0x734c));
    y = func_ov005_02051c1c(*(u16 *)(a2 + 4), (u32 *)(base + 0x754c));
    e = func_ov005_02051ad4(*(u16 *)(a2 + 0xe));
    c = func_ov005_02051ad4(*(u16 *)(a2 + 0xc));
    w = (x >> 3) | 0x50000000;
    w = w | (c << 20);
    w = w | (e << 23);
    *(volatile u32 *)0x040004a8 = w | 0x20000000;
    *(volatile u32 *)0x040004ac = y >> 4;
    *(volatile u32 *)0x040004a4 = 0x001f00c0;
    *(volatile u32 *)0x04000500 = 1;
    v = *(Quad *)data_ov005_020c9694;
    *(volatile u32 *)0x04000488 = v.b;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0xf0000000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.a;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0x10000000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.c;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0x1000f000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.d;
    func_ov005_020569a4(0);
    *(volatile u32 *)0x0400048c = 0xf000f000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000504 = 0;
    return 0;
}