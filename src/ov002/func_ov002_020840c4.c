#include "types.h"

extern u8 data_ov002_02102070[];

s32 func_ov002_020840c4(s32 idx)
{
    u8 *b = *(u8 **)data_ov002_02102070 + 0x36ac;
    u32 other = idx == 0;
    u8 *me = b + idx;
    u8 *you = b + other;
    u8 *me3 = b + 3 + idx;
    u8 *you3 = b + 3 + other;
    if (b[9] == 1)
        return 2;
    (*me)++;
    if (b[2] == 1) {
        if (*me3 == 1)
            return 1;
        if (*you3 == 1)
            *you3 = 0;
        else
            *me3 = 1;
        return 0;
    }
    if (*me == 3) {
        if (*you == 3)
            b[2] = 1;
        return 0;
    }
    return *me > 3;
}