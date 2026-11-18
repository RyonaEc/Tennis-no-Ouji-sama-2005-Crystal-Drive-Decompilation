#include "types.h"

extern u8 data_ov002_02102070[];
extern s32 func_02006c40(void);

void func_ov002_02060060(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    s32 a, b, c, i;

    a = func_02006c40();
    b = func_02006c40();
    c = func_02006c40();
    if (a >= 4 || *(u8 *)(p + 0x90 + a) == 0) {
        a = 0;
        for (i = 0; i < 4; i++) {
            if (*(u8 *)(p + 0x90 + i) != 0) {
                a = i;
                break;
            }
        }
    }
    *(u8 *)(p + 0x105) = a;
    if (c >= 4)
        c = a;
    *(u8 *)(p + 0x106) = b;
    *(u8 *)(p + 0x107) = c;
}
