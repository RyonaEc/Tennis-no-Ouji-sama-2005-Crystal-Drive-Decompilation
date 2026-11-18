#include "types.h"

extern u8 data_ov004_020690a8[];
extern void func_02026af4(s32, s32, s32);

void func_ov004_02051cbc(void)
{
    func_02026af4(*(s32 *)data_ov004_020690a8 + 0x3824, 0, 0x88);
}