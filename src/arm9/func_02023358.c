#include "types.h"

extern u8 data_02042370[];
extern void func_02026738(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
extern void func_020269ec(u32 a0, u32 a1);

void func_02023358(u32 a0, u32 a1, u32 a2)
{
    u32 g = *(u32 *)data_02042370;

    if (g != 0xffffffff)
        func_02026738(g, a0, a1 + 0x68a0000, a2, 0, 0);
    else
        func_020269ec(a0, a1 + 0x68a0000);
}