#include "types.h"

extern void func_020263cc(void);
extern void func_020263b4(void);
extern u32 func_02024b04(void);
extern u32 func_02024bd4(u32);
extern void func_020266c0(u32);
extern void func_02024948(u32);
extern void func_02024894(u32);
extern void func_02026488(u32);
extern void func_01ff840c(void);

void func_020263e0(u32 a0)
{
    u16 n;

    if (*(volatile u16 *)0x027ffc40 == 2)
        func_020263cc();
    n = (u16)func_02024b04();
    if (func_02024bd4(n) != 0)
    {
        do
        {
            func_020263b4();
        } while (func_02024bd4(n) != 0);
    }
    func_020266c0(0);
    func_020266c0(1);
    func_020266c0(2);
    func_020266c0(3);
    func_02024948(0x40000);
    func_02024894(-1);
    *(volatile u32 *)0x027ffc20 = a0;
    func_02026488(0x10);
    func_01ff840c();
}