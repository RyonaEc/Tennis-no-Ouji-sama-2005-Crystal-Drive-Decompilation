#include "types.h"

typedef struct {
    u32 a, b, c, d;
} Quad;

extern u8 data_ov003_020eb720[];
extern u32 data_ov003_020eb174[];
extern u32 func_ov003_020555d4(void);
extern s32 func_ov003_0205578c(u32, u32 *);
extern u32 func_ov003_02055744(u32, u32 *);
extern u32 func_ov003_020555fc(u32);
extern void func_ov003_020641e4(u32);

s32 func_ov003_02063f68(u32 a0, u32 a1, u8 *a2)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u32 x, y, e, c, w;
    Quad v;

    func_ov003_020555d4();
    x = func_ov003_0205578c(*(u16 *)(a2 + 4), (u32 *)(base + 0xb6dc));
    y = func_ov003_02055744(*(u16 *)(a2 + 4), (u32 *)(base + 0xb7dc));
    e = func_ov003_020555fc(*(u16 *)(a2 + 0xe));
    c = func_ov003_020555fc(*(u16 *)(a2 + 0xc));
    w = (x >> 3) | 0x50000000;
    w = w | (c << 20);
    w = w | (e << 23);
    *(volatile u32 *)0x040004a8 = w | 0x20000000;
    *(volatile u32 *)0x040004ac = y >> 4;
    *(volatile u32 *)0x040004a4 = 0x001f00c0;
    *(volatile u32 *)0x04000500 = 1;
    v = *(Quad *)data_ov003_020eb174;
    *(volatile u32 *)0x04000488 = v.b;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0xf0000000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.a;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0x10000000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.c;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0x1000f000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.d;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0xf000f000;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000504 = 0;
    return 0;
}