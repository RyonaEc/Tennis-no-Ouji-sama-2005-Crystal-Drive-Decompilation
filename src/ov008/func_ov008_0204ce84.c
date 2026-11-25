#include "types.h"

extern u32 data_ov008_0204f440[];
extern s32 func_02006448(void);
extern u32 func_ov008_0204ed50(void);

void func_ov008_0204ce84(void)
{
    u8 *g = (u8 *)data_ov008_0204f440[0];
    s32 v;
    u32 r;

    if (func_02006448() == 1) {
        if (g[0x33] < 3) {
            u8 *p = g + 0x33;

            *p = *p + 1;
        } else
            g[0x33] = 0;
        if (g[0x33] < 2) {
            g[0x31] = 0x40;
            g[0x32] = 1;
        } else {
            g[0x31] = 0;
            *(s8 *)(g + 0x32) = -1;
        }
    } else {
        if (g[0x31] == 0 || g[0x31] == 0x40) {
            s8 *p = (s8 *)(g + 0x32);

            *p = *p * 0xffffffffu;
        }
        {
            u8 *p = g + 0x31;

            *p = *p + *(s8 *)(g + 0x32);
        }
    }
    v = (s32)(g[0x31] << 4) / 64;
    r = func_ov008_0204ed50();
    *(u16 *)(r + 6) = 0x150;
    *(u16 *)(r + 8) = (0x10 - v) | (v << 8);
}