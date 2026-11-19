#include "types.h"

extern void func_02022368(u32);

void func_02022704(void)
{
    func_02022368(0x04000400);
    while (*(volatile u32 *)0x04000600 & 0x8000000)
        ;
}