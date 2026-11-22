#include "types.h"

extern u32 func_02033170(u32);

void func_02032f90(u8 *p)
{
    volatile u16 tmp;

    if (func_02033170(1) != 0 && *(u16 *)(p + 4) != 0) {
        *(u16 *)0x027fff72 = *(u16 *)(p + 2);
        *(u16 *)0x09FE2FFE = *(u16 *)(p + 2);
        tmp = *(volatile u16 *)0x09FE20F0;
        tmp = *(volatile u16 *)0x09FE20F0;
        tmp = *(volatile u16 *)0x09FE20F0;
    }
    *(u16 *)0x04000204 = *(u16 *)p;
}