#include "types.h"

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
extern void func_02020620(void);
extern void func_0202060c(void);

void func_02004d7c(void)
{
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
    func_02020620();
    func_0202060c();
    *(u32 *)0x04000000 = *(u32 *)0x04000000 & ~0x38000000;
    *(u32 *)0x04000000 = *(u32 *)0x04000000 & ~0x07000000;
    *(u32 *)0x04000000 = *(u32 *)0x04000000 & ~0x0000e000;
    *(u32 *)0x04001000 = *(u32 *)0x04001000 & ~0x0000e000;
}