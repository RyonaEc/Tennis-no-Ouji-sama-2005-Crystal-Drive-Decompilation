#include "types.h"

extern u8 data_02042370[];
extern void func_020268e0(s32, s32, s32, s32);
extern void func_020269ec(s32, s32);

void func_02023f20(s32 a0, s32 a1, s32 a2)
{
    s32 id = *(s32 *)data_02042370;

    if (id != -1)
        func_020268e0(id, a0, a1 + 0x7000000, a2);
    else
        func_020269ec(a0, a1 + 0x7000000);
}