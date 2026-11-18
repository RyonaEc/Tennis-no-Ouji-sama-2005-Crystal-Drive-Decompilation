#include "types.h"

extern u8 data_ov004_020690a8[];
extern s32 func_02018e8c(s32, s32);

s32 func_ov004_0204fc18(s32 a)
{
    return func_02018e8c(*(s32 *)data_ov004_020690a8 + 0x35b8, a);
}