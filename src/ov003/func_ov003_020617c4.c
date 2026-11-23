#include "types.h"

extern u8 data_ov003_020e6fe4[];
extern u8 data_ov003_020eb720[];
extern u8 data_ov003_020eb724[];
extern u8 data_ov003_020eb728[];
extern u8 data_02042a68[];
extern void func_02021af0(volatile u32 *, u32, u32);
extern void func_020240b4(u8 *, u32, u32);
extern void func_0202404c(u8 *, u32, u32);
extern void func_02004d7c(void);
extern u32 func_02017b60(u32, u32, u32);
extern void func_020263cc(void);
extern void func_02026a04(u32, u32, u32);
extern void func_02020af0(u32);
extern void func_020206cc(void);
extern void func_020206b8(void);
extern void func_02020694(void);
extern void func_02020670(void);
extern void func_0202065c(void);
extern void func_02020648(void);
extern void func_02020634(void);
extern void func_020205f8(void);
extern void func_020205e4(void);
extern void func_020205bc(void);
extern void func_02020594(void);
extern void func_0202060c(void);
extern void func_ov003_0206127c(void);

void func_ov003_020617c4(void)
{
    volatile u32 C;
    volatile u32 E;
    volatile u32 A;
    volatile u32 B;
    volatile u32 D;
    volatile u32 F;
    u32 h;

    func_02021af0((volatile u32 *)0x04000050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04001050, 0x3f, ~0xf);
    func_020240b4(data_ov003_020e6fe4, 0, 0x20);
    func_0202404c(data_ov003_020e6fe4, 0, 0x20);
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x1f00u;
    *(volatile u32 *)0x04001000 = *(volatile u32 *)0x04001000 & ~0x1f00u;
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x38000000u;
    *(volatile u32 *)0x04000000 = *(volatile u32 *)0x04000000 & ~0x7000000u;
    *(volatile u8 *)0x0400004d = 0;
    *(volatile u8 *)0x0400104d = 0;
    *(volatile u8 *)0x0400004c = 0;
    *(volatile u8 *)0x0400104c = 0;
    func_020240b4(data_ov003_020e6fe4, 0, 0x20);
    func_0202404c(data_ov003_020e6fe4, 0, 0x20);
    func_02004d7c();
    h = func_02017b60(*(u32 *)data_02042a68, 0xd8dc, 4);
    *(u32 *)data_ov003_020eb720 = h;
    if (h == 0)
        func_020263cc();
    A = 0;
    func_02026a04(A, *(u32 *)data_ov003_020eb720, 0xd8dc);
    h = func_02017b60(*(u32 *)data_02042a68, 0x180000, 4);
    *(u32 *)data_ov003_020eb724 = h;
    if (h == 0)
        func_020263cc();
    h = func_02017b60(*(u32 *)data_02042a68, 0x10000, 4);
    *(u32 *)data_ov003_020eb728 = h;
    if (h == 0)
        func_020263cc();
    func_020206cc();
    func_020206b8();
    func_02020694();
    func_02020670();
    func_0202065c();
    func_02020648();
    func_02020634();
    func_020205f8();
    func_020205e4();
    func_020205bc();
    func_02020594();
    func_0202060c();
    func_02020af0(0x1ff);
    B = 0;
    func_02026a04(B, 0x6800000, 0xa4000);
    func_0202060c();
    C = 0xc0;
    func_02026a04(C, 0x7000000, 0x400);
    D = 0;
    func_02026a04(D, 0x5000000, 0x400);
    E = 0xc0;
    func_02026a04(E, 0x07000400, 0x400);
    F = 0;
    func_02026a04(F, 0x05000400, 0x400);
    func_ov003_0206127c();
}