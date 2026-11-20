#include "types.h"

extern u8 data_02042b04[];
extern u8 data_02042b80[];
extern void func_0201657c(void);
extern void func_02017710(u32);
extern u32 func_020303a0(u32, u8 *);
extern void func_020176f0(void);

u32 func_020165a8(void)
{
    u32 v = *(u32 *)data_02042b04;

    if (v == 6) return 1;
    if (v != 4) return 0;
    func_02017710(6);
    if (func_020303a0((u32)func_0201657c, data_02042b80) == 2) return 1;
    func_020176f0();
    return 0;
}