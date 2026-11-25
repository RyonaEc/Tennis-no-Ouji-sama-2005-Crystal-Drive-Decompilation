#include "types.h"

extern u32 data_02042b04;
extern u32 data_02042b0c;
extern u32 data_02042b14;
extern u32 data_02042b18;
extern u32 data_02042b24;
extern u32 func_02017710(u32 a0);
extern u32 func_020176f0(u32 a0);
extern u32 func_02017544(void);
extern s32 func_0202f3c4(u32 a0, u32 a1, u32 a2, u32 a3, u16 a4, u16 a5);
extern void func_02016bd4(void);

s32 func_02016d1c(void)
{
    u16 a;
    u16 b;
    s32 r;

    if (data_02042b04 - 4 <= 2)
        return 1;
    func_02017710(4);
    a = (u16)data_02042b18;
    b = (u16)data_02042b24;
    r = func_0202f3c4((u32)func_02016bd4, data_02042b0c, b, data_02042b14, a, 1);
    if (r == 2)
        return 1;
    func_020176f0(r);
    func_02017544();
    return 0;
}