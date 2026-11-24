#include "types.h"

extern u8 data_ov003_020eb720[];

u32 func_ov003_02071dfc(u32 a0)
{
    u8 *p = *(u8 **)data_ov003_020eb720 + 0xb8dc;
    u32 r = 0;
    s16 v[2] = {0, 0};
    s16 d;
    u32 s;

    if (a0 == 0) {
        v[0] = *(u16 *)(p + 0x14);
        v[1] = *(u16 *)(p + 0x16);
        d = v[0] - v[1];
        if (v[0] == 3 && v[1] == 4)
            s = 3;
        else if (v[0] == 4 && v[1] == 3)
            s = 3;
        else if (d == 1 || d == -1)
            s = 0;
        else if (d == 2 || d == -2)
            s = 1;
        else
            s = 2;
        if (s == 0)
            r = 0x14;
        if (s == 1) {
            if (v[0] == 4 || v[1] == 4)
                r = 0x22;
            else
                r = 0x28;
        }
        if (s == 2)
            r = 0x36;
        if (s == 3)
            r = 0xe;
    }
    return r;
}