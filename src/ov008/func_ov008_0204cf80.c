#include "types.h"

typedef struct {
    u8 pad[6];
    u16 h6;
    u16 h8;
} R;

extern u8 *data_ov008_0204f440;
extern u8 *func_ov008_0204ed50(void);

u32 func_ov008_0204cf80(u32 arg)
{
    volatile u8 *p = data_ov008_0204f440;
    R *r;
    s32 t;

    if (p[0x30] < 0xc)
        p[0x30] += 1;
    t = p[0x30] * 16 / 12;
    if (arg == 0)
        t = 16 - t;
    r = (R *)func_ov008_0204ed50();
    r->h6 = 0x150;
    r->h8 = (16 - t) | (t << 8);
    return p[0x30] == 0xc;
}