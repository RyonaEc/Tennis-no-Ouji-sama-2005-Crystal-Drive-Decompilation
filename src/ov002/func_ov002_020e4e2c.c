#include "types.h"

extern u32 func_ov002_02062dbc(u32);
extern void func_ov002_02085170(u8, u32, s32);
extern void func_ov002_020589d0(u8);
void func_ov002_020e4e2c(u32 a0)
{
    u32 v = func_ov002_02062dbc(0xc);
    func_ov002_02085170(a0, 1 << v, -1);
    func_ov002_020589d0(a0);
}
