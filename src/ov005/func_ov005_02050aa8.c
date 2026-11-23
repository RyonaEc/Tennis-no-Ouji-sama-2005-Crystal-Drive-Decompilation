#include "types.h"

extern u8 data_ov005_020790a0[];
extern u8 data_ov005_020c9740[];
extern u8 data_ov005_020c9744[];
extern u8 data_02042a68[];
extern void func_02021af0(volatile u32 *, u32, u32);
extern void func_020240b4(u8 *, u32, u32);
extern void func_0202404c(u8 *, u32, u32);
extern u32 func_02017b60(u32, u32, u32);
extern void func_020263cc(void);
extern void func_02026a04(u32, u32, u32);
extern void func_02004d7c(void);
extern void func_02020af0(u32);
extern void func_0202060c(void);
extern void func_ov005_02050974(void);

void func_ov005_02050aa8(void)
{
    volatile u32 p;
    volatile u32 q;
    volatile u32 r;
    volatile u32 s;
    volatile u32 t;
    volatile u32 u;
    u32 h;

    func_02021af0((volatile u32 *)0x04000050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04001050, 0x3f, ~0xf);
    func_020240b4(data_ov005_020790a0, 0, 0x20);
    func_0202404c(data_ov005_020790a0, 0, 0x20);
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x1f00u;
    *(volatile u32 *)0x04001000 = *(volatile u32 *)0x04001000 & ~0x1f00u;
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x38000000u;
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x7000000u;
    *(volatile u8 *)0x0400004d = 0;
    *(volatile u8 *)0x0400104d = 0;
    *(volatile u8 *)0x0400004c = 0;
    *(volatile u8 *)0x0400104c = 0;
    func_020240b4(data_ov005_020790a0, 0, 0x20);
    func_0202404c(data_ov005_020790a0, 0, 0x20);
    h = func_02017b60(*(u32 *)data_02042a68, 0x934c, 4);
    *(u32 *)data_ov005_020c9740 = h;
    if (h == 0)
        func_020263cc();
    r = 0;
    func_02026a04(r, *(u32 *)data_ov005_020c9740, 0x934c);
    h = func_02017b60(*(u32 *)data_02042a68, 0x188000, 4);
    *(u32 *)data_ov005_020c9744 = h;
    if (h == 0)
        func_020263cc();
    func_02004d7c();
    func_02020af0(0x1ff);
    s = 0;
    func_02026a04(s, 0x6800000, 0xa4000);
    func_0202060c();
    p = 0xc0;
    func_02026a04(p, 0x7000000, 0x400);
    t = 0;
    func_02026a04(t, 0x5000000, 0x400);
    q = 0xc0;
    func_02026a04(q, 0x07000400, 0x400);
    u = 0;
    func_02026a04(u, 0x05000400, 0x400);
    func_ov005_02050974();
}