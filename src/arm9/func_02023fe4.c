#include "types.h"

extern u8 data_02042370[];
extern void func_0202688c(s32, s32, s32, s32);
extern void func_020269d0(s32, s32);

void func_02023fe4(s32 a0, s32 a1, s32 a2)
{
    s32 id = *(s32 *)data_02042370;

    if (id != -1)
        func_0202688c(id, a0, a1 + 0x5000200, a2);
    else
        func_020269d0(a0, a1 + 0x5000200);
}