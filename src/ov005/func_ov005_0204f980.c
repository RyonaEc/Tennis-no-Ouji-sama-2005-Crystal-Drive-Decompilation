#include "types.h"

extern u8 data_ov005_020790a0[];
extern u8 data_ov005_020c9744[];
extern u8 data_ov005_020c9740[];
extern u8 data_02042a68[];
extern void func_02021af0(volatile u32 *, u32, u32);
extern void func_020240b4(u8 *, u32, u32);
extern void func_0202404c(u8 *, u32, u32);
extern s32 func_02017b20(u32, u32);
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

void func_ov005_0204f980(void)
{
    func_02021af0((volatile u32 *)0x04000050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04001050, 0x3f, ~0xf);
    *(volatile u32 *)0x04000000 &= ~0x1f00;
    *(volatile u32 *)0x04001000 = *(volatile u32 *)0x04001000 & ~0x1f00u;
    func_02021af0((volatile u32 *)0x04000050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04001050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04000050, 0x3f, ~0xf);
    func_02021af0((volatile u32 *)0x04001050, 0x3f, ~0xf);
    func_020240b4(data_ov005_020790a0, 0, 0x20);
    func_0202404c(data_ov005_020790a0, 0, 0x20);
    *(volatile u32 *)0x04000000 &= ~0x38000000;
    *(volatile u32 *)0x04000000 &= ~0x7000000;
    func_02017b20(*(u32 *)data_02042a68, *(u32 *)data_ov005_020c9744);
    func_02017b20(*(u32 *)data_02042a68, *(u32 *)data_ov005_020c9740);
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
}