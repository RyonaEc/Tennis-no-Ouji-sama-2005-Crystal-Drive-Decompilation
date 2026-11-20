#include "types.h"

extern u8 data_02042b04[];
extern void func_02016aa8(void);
extern void func_02017710(u32);
extern u32 func_0203030c(u32);
extern void func_020176f0(void);

u32 func_02016518(void)
{
    if (*(u32 *)data_02042b04 != 6) return 0;
    func_02017710(3);
    if (func_0203030c((u32)func_02016aa8) == 0) return 1;
    func_020176f0();
    return 0;
}