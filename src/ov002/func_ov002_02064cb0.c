#include "types.h"

extern u32 data_ov002_02102070[];
extern s32 func_ov002_02062dbc(u32 a0);

void func_ov002_02064cb0(u32 a0)
{
    u32 base = data_ov002_02102070[0];
    u32 p = base + 0x1744 + a0 * 0x38;
    s32 r = func_ov002_02062dbc(0x100);

    switch (*(u8 *)p) {
    case 0:
        if (r == 0)
            *(u32 *)(p + 0x24) |= 0x800;
        break;
    case 1:
        if (r < 8)
            *(u32 *)(p + 0x24) |= 0x800;
        break;
    case 2:
        if (r < 0x10)
            *(u32 *)(p + 0x24) |= 0x800;
        break;
    }
}