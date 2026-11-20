#include "types.h"

extern u8 data_02042b0c[];
extern u8 data_02042b14[];
extern u8 data_02042b18[];
extern u8 data_02042b24[];
extern s32 func_02016628(void);
extern s32 func_0202f3c4(s32 (*)(void), u32, u32, u32, u32, u32);
extern void func_020176f0(void);

s32 func_02016790(void)
{
    if (func_0202f3c4(func_02016628, *(u32 *)data_02042b0c,
                      (u16)*(u32 *)data_02042b24, *(u32 *)data_02042b14,
                      (u16)*(u32 *)data_02042b18, 1) == 2) return 1;
    func_020176f0();
    return 0;
}