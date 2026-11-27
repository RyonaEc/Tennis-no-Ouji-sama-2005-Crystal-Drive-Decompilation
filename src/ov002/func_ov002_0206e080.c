#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fc78e[];
extern u8 data_ov002_020fc790[];
extern void func_020064c0(s32 sel, s32 arg);

void func_ov002_0206e080(u32 a0, u32 a1)
{
    u8 *t = *(u8 **)data_ov002_02102070 + 0x2e4 + a1 * 0x158;
    u16 v = 0;

    switch (a0) {
    case 1:
        if (t[0] != 0)
            v = *(u16 *)(data_ov002_020fc78e + t[0] * 0x1e);
        break;
    case 2:
        if (t[0] != 0)
            v = *(u16 *)(data_ov002_020fc78e + t[0] * 0x1e);
        break;
    case 5:
        if (t[0] != 0)
            v = *(u16 *)(data_ov002_020fc790 + t[0] * 0x1e);
        break;
    }
    if (v == 0)
        return;
    func_020064c0((u32)t[2] >> 1, v);
}