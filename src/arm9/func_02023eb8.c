#include "types.h"

extern u8 data_02042370[];
extern void func_020268e0(s32, u32, u32, u32);
extern void func_020269ec(u32, u32, u32, u32);

void func_02023eb8(u32 a0, u32 a1, u32 a2)
{
    s32 v = *(s32 *)data_02042370;

    if (v != -1)
    {
        func_020268e0(v, a0, a1 + 0x07000400, a2);
        return;
    }
    func_020269ec(a0, a1 + 0x07000400, a2, a2);
}