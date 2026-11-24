#include "types.h"

extern u32 func_020257e4(void);
extern u32 data_02048f8c;
extern u32 data_02048f84;
extern void func_020260b8(void);

void func_0202623c(void)
{
    u8 *base = (u8 *)0x027c3000;
    u32 v = *(u32 *)0x027ffd9c;

    if (v >= 0x02600000 && v < 0x02800000)
        data_02048f8c = v;
    else
        data_02048f8c = 0;
    if (data_02048f8c == 0 || (func_020257e4() & 0x40000000) == 0) {
        *(u32 *)0x027ffd9c = (u32)func_020260b8;
        *(u32 *)(base + 0xfdc) = (u32)func_020260b8;
    }
    data_02048f84 = 0;
}