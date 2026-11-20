#include "types.h"

extern u8 data_02042370[];
extern u32 func_02019444(u8 *);
extern u32 func_02019480(u8 *);
extern void func_02025c5c(u32, u32);
extern void func_02026934(u32, u32, u32, u32);
extern void func_020190f8(u32, u32, u32);

void func_02018da8(u8 *a0)
{
    u32 x;
    u32 y;

    if (*(u32 *)(a0 + 0x14) != 0)
    {
        x = func_02019444(a0 + 4);
        y = func_02019480(a0);
        func_02025c5c(y, x);
        func_02026934(*(u32 *)data_02042370, y, 0xc0, x);
    }
    else
    {
        func_020190f8(*(u32 *)a0, *(u16 *)(a0 + 4), *(u16 *)(a0 + 6));
    }
    *(u16 *)(a0 + 8) = *(u16 *)(a0 + 4);
    *(u16 *)(a0 + 0xe) = *(u16 *)(a0 + 0xa);
}