#include "types.h"

extern u8 data_ov002_02102070[];
void func_ov002_02061910(void)
{
    u8 *g = *(u8 **)data_ov002_02102070;
    u16 i;
    u8 a[2];
    u8 b[2];
    u32 na = 0;
    u32 nb = 0;
    u8 t;
    u8 *tbl;

    for (i = 0; i < 4; i++) {
        if (*(u8 *)(g + 0x94 + i) % 2 == 0) {
            a[na] = i;
            na = (na + 1) & 0xff;
            if (na > 1)
                na = 1;
        } else {
            b[nb] = i;
            nb = (nb + 1) & 0xff;
            if (nb > 1)
                nb = 1;
        }
    }
    tbl = g + 0x94;
    t = tbl[a[0]];
    tbl[a[0]] = tbl[a[1]];
    tbl[a[1]] = t;
    t = tbl[b[0]];
    tbl[b[0]] = tbl[b[1]];
    tbl[b[1]] = t;
    g[0x10b]++;
}