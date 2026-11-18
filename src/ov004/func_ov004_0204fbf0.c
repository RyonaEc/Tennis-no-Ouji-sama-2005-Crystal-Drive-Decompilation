#include "types.h"

extern u8 data_ov004_020690a8[];
extern void func_02018e8c(s32 a0, s32 a1);

void func_ov004_0204fbf0(s32 a0)
{
    func_02018e8c(*(s32 *)data_ov004_020690a8 + 0x35d4, a0);
}
