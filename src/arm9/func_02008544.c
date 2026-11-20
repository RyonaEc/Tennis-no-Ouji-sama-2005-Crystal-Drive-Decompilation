#include "types.h"

extern u8 data_0203cb6c[];
extern u32 func_0200ff30(u32);
extern u32 func_020100b4(u32);

void func_02008544(s32 a0, u16 *a1)
{
    u16 i;
    u8 *p;
    u16 v;
    u16 n;

    i = 0;
    *a1 = 0;
    if (a0 >= 9) return;
    while (i < 0x40)
    {
        p = data_0203cb6c + i * 0x14;
        if (a0 == *(u16 *)(p + 0x10))
        {
            v = *(u16 *)(p + 0xe);
            if (a0 == 8)
            {
                if (func_0200ff30(v) == 0) goto next;
            }
            else
            {
                if (func_020100b4(v) == 0) goto next;
            }
            n = *a1;
            *a1 = n + 1;
            *(u16 *)((u8 *)a1 + n * 2 + 2) = v;
        }
next:
        i = (u16)(i + 1);
    }
}