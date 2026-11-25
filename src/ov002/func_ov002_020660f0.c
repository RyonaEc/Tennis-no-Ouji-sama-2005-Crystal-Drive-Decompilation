#include "types.h"

extern u32 data_ov002_02102070[];
extern u32 func_ov002_02062dbc(u32 a0);

s32 func_ov002_020660f0(s32 a0)
{
    u8 *p = (u8 *)data_ov002_02102070[0] + a0 * 0x38;
    s32 ret = 0;
    u8 m = *(u8 *)(p + 0x1744);

    switch (m) {
    case 0:
        if (func_ov002_02062dbc(0x100) < 0x20)
            ret = 1;
        break;
    case 1:
        if (func_ov002_02062dbc(0x100) < 0x80)
            ret = 1;
        break;
    case 2:
        if ((s32)func_ov002_02062dbc(0x100) < 0x100)
            ret = 1;
        break;
    }
    return ret;
}