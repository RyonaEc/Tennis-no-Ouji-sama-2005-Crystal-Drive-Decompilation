#include "types.h"

extern u32 data_02042370;
extern u32 data_02048c94;
extern void func_02026738(u32, u32, u32, u32, u32, u32);
extern void func_020269ec(u32, u32, u32);

void func_0202352c(u32 a0, u32 a1, u32 a2)
{
    u32 addr = data_02048c94 + a1;

    if (data_02042370 != 0xffffffff)
        func_02026738(data_02042370, a0, addr, a2, 0, 0);
    else
        func_020269ec(a0, addr, a2);
}