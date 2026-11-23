#include "types.h"

typedef struct {
    u32 a, b, c, d;
} Quad;

extern u8 data_ov003_020eb720[];
extern u32 data_ov003_020eb134[];
extern u32 func_ov003_020555d4(void);
extern s32 func_ov003_0205578c(u32, u32 *);
extern u32 func_ov003_02055744(u32, u32 *);
extern void func_ov003_020641e4(u32);

void func_ov003_02062ec0(u32 a0)
{
    u8 *base = *(u8 **)data_ov003_020eb720;
    u32 x, y;
    Quad v;

    func_ov003_020555d4();
    x = func_ov003_0205578c(a0, (u32 *)(base + 0xb6dc));
    y = func_ov003_02055744(a0, (u32 *)(base + 0xb7dc));
    *(volatile u32 *)0x040004a8 = 0x6d300000 | (x >> 3);
    *(volatile u32 *)0x040004ac = y >> 4;
    *(volatile u32 *)0x040004a4 = 0x001f00c0;
    *(volatile u32 *)0x04000500 = 1;
    v = *(Quad *)data_ov003_020eb134;
    *(volatile u32 *)0x04000488 = v.b;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0x400;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.a;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0x05540400;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.c;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0x0554fc00;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000488 = v.d;
    func_ov003_020641e4(0);
    *(volatile u32 *)0x0400048c = 0xfc00;
    *(volatile u32 *)0x0400048c = 0;
    *(volatile u32 *)0x04000504 = 0;
}