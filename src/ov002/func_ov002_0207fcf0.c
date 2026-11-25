#include "types.h"

extern u8 data_ov002_020fd074[];

u32 func_ov002_0207fcf0(s32 a0, s32 a1, s32 a2)
{
    u8 *p = &data_ov002_020fd074[a1 * 9];
    u8 t = p[a2];

    switch (a1) {
    case 2:
        if (a2 == 6)
            t = 4;
        break;
    case 0x1e:
        if (a2 == 0)
            t = 1;
        break;
    case 0x20:
        if (a2 == 0)
            t = 1;
        break;
    case 0x28:
        if (a2 == 0 || a2 == 1 || a2 == 4)
            t = 4;
        break;
    }
    return a0 >= t;
}