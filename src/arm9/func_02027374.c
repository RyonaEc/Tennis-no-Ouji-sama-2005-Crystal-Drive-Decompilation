#include "types.h"

extern void func_020273e0(void);
extern void func_02027f74(u32, u32);
extern u32 func_0202585c(void);
extern u32 func_02027f4c(u32, u32);
extern void func_020263c0(u32);

void func_02027374(void)
{
    func_02027f74(7, (u32)func_020273e0);
    if (func_0202585c() != 0) return;
    if (func_02027f4c(7, 1) != 0) return;
    do
    {
        func_020263c0(0x64);
    } while (func_02027f4c(7, 1) == 0);
}