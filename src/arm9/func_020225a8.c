#include "types.h"

extern u32 func_02022428(u32 *a);
extern u32 func_020223fc(u32 *a);

void func_020225a8(void)
{
    s32 levelPV;
    s32 levelPJ;

    *(volatile u32 *)0x04000600 |= 0x8000;

    while (func_02022428((u32 *)&levelPV))
        ;

    while (func_020223fc((u32 *)&levelPJ))
        ;

    *(volatile u32 *)0x04000440 = 3;
    *(volatile u32 *)0x04000454 = 0;
    *(volatile u32 *)0x04000440 = 0;

    if (levelPJ != 0)
        *(volatile u32 *)0x04000448 = (u32)levelPJ;

    *(volatile u32 *)0x04000440 = 2;
    *(volatile u32 *)0x04000448 = (u32)levelPV;
    *(volatile u32 *)0x04000454 = 0;
}