#include "types.h"

extern void func_02017710(u32);
extern void func_020176f0(u32);
extern u8 data_02042af0[];
extern u8 data_02042b08[];
extern u8 data_02042b20[];
extern void func_02015c9c(void);

void func_02015e14(u32 a0)
{
    func_02017710(2);
    func_020176f0(0x1a);
    *(u16 *)data_02042af0 = a0;
    *(u32 *)data_02042b08 = 0;
    if (a0 != 3) *(u32 *)data_02042b20 = 0;
    func_02015c9c();
}