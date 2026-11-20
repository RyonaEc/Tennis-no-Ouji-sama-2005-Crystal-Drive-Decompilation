#include "types.h"

extern u8 data_02042370[];
extern void func_020267cc(u32, u32, u32, u32, u32, u32);
extern void func_02026934(u32, u32, u32, u32);
extern void func_020048e8(u32, u32, u32);

void func_02022454(void)
{
    s32 i;

    if (*(u32 *)data_02042370 != 0xffffffff) {
        u32 h = *(u32 *)data_02042370;

        func_020267cc(h, 0x4000330, 0, 0x10, 0, 0);
        func_02026934(*(u32 *)data_02042370, 0x4000360, 0, 0x60);
    } else {
        volatile u32 z1 = 0;

        func_020048e8(z1, 0x4000330, 0x10);
        {
            volatile u32 z2 = 0;

            func_020048e8(z2, 0x4000360, 0x60);
        }
    }
    for (i = 0; i < 0x20; i++) *(volatile u32 *)0x40004d0 = 0;
}