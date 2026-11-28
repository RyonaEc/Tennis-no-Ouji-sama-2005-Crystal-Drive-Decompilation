#include "types.h"

extern u8 data_ov005_020c9740[];
extern void func_02008544(s32 a0, u16 *a1);
extern void func_ov005_0205a218(u32 a0, u32 a1);

s32 func_ov005_0205a7ec(u16 *p)
{
    u8 *b = *(u8 **)data_ov005_020c9740 + 0x774c;
    u16 buf[0x28];
    u16 n = *p;
    u16 i;

    if (n == 9) {
        func_02008544(*(u16 *)(b + 0x358 + *(u16 *)(b + 0x3e2) * 2), buf);
        if (buf[0] != 0) {
            for (i = 0; i < buf[0]; i++)
                *(u16 *)(b + 0x370 + i * 2) = ((u16 *)((u8 *)buf + i * 2))[1];
        }
        if (*(u16 *)(b + 0x358 + *(u16 *)(b + 0x3e2) * 2) == 8)
            func_ov005_0205a218(1, 0);
        else
            func_ov005_0205a218(0, 0);
    }
    if (n != 0)
        n = n - 1;
    *p = n;
    return 0;
}