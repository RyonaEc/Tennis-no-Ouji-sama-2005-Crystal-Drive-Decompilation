#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042af8[];
extern u8 data_02042af4[];
extern s32 func_02016dd4(void);
extern s32 func_0202ed74(s32 (*)(void));
extern void func_020176f0(void);
extern void func_02017544(void);

s32 func_02016ee8(void)
{
    if (*(u32 *)data_02042b04 - 4 <= 2) return 1;
    if (func_0202ed74(func_02016dd4) != 2) {
        func_020176f0();
        func_02017544();
        return 0;
    }
    *(u16 *)data_02042af8 = 0;
    *(u16 *)data_02042af4 = 1;
    return 1;
}