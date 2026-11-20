#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042afc[];
extern void func_02017710(u32);
extern u32 func_02017544(void);
extern u32 func_02016518(void);
extern u32 func_020164d4(void);
extern u32 func_02016b04(void);
extern u32 func_02016a64(void);

u32 func_0201746c(void)
{
    u32 s = *(u32 *)data_02042b04;

    if (s == 1) return 0;
    if (s != 6 && s != 5 && s != 4)
    {
        func_02017710(3);
        return func_02017544();
    }
    switch (*(u32 *)data_02042afc)
    {
    case 3:
        return func_02016518();
    case 1:
    case 5:
        return func_020164d4();
    case 2:
        return func_02016b04();
    case 0:
    case 4:
        return func_02016a64();
    }
    return 0;
}