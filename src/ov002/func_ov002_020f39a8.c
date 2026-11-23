#include "types.h"

extern u8 *data_ov002_02102070;
extern s32 func_ov002_02062dbc(u32);
extern void func_ov002_02066174(u32, u8 *, u8 *);

u32 func_ov002_020f39a8(u32 a0, u32 a1)
{
    u8 *base = data_ov002_02102070;
    s32 *p = (s32 *)(base + 0x1744 + a0 * 0x38);
    u32 r = 0;

    switch (a1) {
    case 0:
        p[0xc] = 0;
        p[0xd] = func_ov002_02062dbc(0x20) + 0x20;
        break;
    case 1:
        if (p[0xc] < p[0xd]) {
            p[0xc] = p[0xc] + 1;
        } else {
            func_ov002_02066174(a0, (u8 *)p + 8, (u8 *)p + 9);
            r = 1;
        }
        break;
    }
    return r;
}