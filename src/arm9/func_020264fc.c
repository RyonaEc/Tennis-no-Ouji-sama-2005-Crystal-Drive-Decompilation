#include "types.h"

extern u8 data_02049024[];
extern void func_02027d68(void);
extern u32 func_02027f4c(u32, u32);
extern void func_02027f74(u32, u32);
extern void func_020264b8(void);

void func_020264fc(void)
{
    if (*(u16 *)data_02049024 != 0) return;
    *(u16 *)data_02049024 = 1;
    func_02027d68();
    while (func_02027f4c(0xc, 1) == 0)
        ;
    func_02027f74(0xc, (u32)func_020264b8);
}