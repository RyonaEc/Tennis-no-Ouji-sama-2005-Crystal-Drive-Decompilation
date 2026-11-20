#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042afc[];
extern u8 data_02043720[];
extern u8 data_02042da0[];
extern u32 func_0202fb2c(u32, u32, u32);
extern void func_020176f0(void);

u32 func_020173a0(u32 a0)
{
    u32 r;

    if (*(u32 *)data_02042b04 != 5) return 0;
    if (*(u32 *)data_02042afc != 5 && *(u32 *)data_02042afc != 4) return 0;
    r = func_0202fb2c((u32)data_02043720, a0, (u32)data_02042da0);
    switch (r)
    {
    case 5:
        func_020176f0();
        return 2;
    case 0:
    case 7:
        break;
    default:
        func_020176f0();
        return 1;
    }
    return 0;
}