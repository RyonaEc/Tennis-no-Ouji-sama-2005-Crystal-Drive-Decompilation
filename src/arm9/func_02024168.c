#include "types.h"

extern u32 data_02042370;
extern u32 data_02048cbc;
extern void func_02026738(u32, u32, u32, u32, u32, u32);
extern void func_020269ec(u32, u32, u32);

void func_02024168(u32 a0, u32 a1)
{
    u32 addr = data_02048cbc + 0x20000;

    if (data_02042370 != 0xffffffff)
        func_02026738(data_02042370, a0, addr, a1, 0, 0);
    else
        func_020269ec(a0, addr, a1);
}