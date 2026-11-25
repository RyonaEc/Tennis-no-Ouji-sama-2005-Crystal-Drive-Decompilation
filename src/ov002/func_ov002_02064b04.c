#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_ov002_02062dbc(u32 a0);

s32 func_ov002_02064b04(s32 a0)
{
    u8 *p = (u8 *)data_ov002_02102070[0] + 0x1744 + a0 * 0x38;
    s32 r;
    s32 ret;

    r = func_ov002_02062dbc(0x100);
    ret = 0;
    switch (p[0]) {
    case 0:
        break;
    case 1:
        if (r < 0xc8)
            ret = 1;
        break;
    case 2:
        ret = 1;
        break;
    }
    return ret;
}