#include "types.h"

extern u16 data_0204236c;
extern u16 data_02048c70;

void func_02020240(u32 a, u32 b, u32 c)
{
    u32 disp = *(volatile u32 *)0x04000000;
    u16 flag = data_0204236c;

    data_02048c70 = (u16)a;
    if (flag == 0)
        a = 0;
    *(volatile u32 *)0x04000000 = (disp & 0xfff0fff0) | (a << 16) | b | (c << 3);
    if (data_02048c70 == 0)
        data_0204236c = 0;
}