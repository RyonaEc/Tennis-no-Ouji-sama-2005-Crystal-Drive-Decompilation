#include "types.h"

extern u8 data_ov002_02102070[];
extern u8 data_ov002_020fb544[];
extern void func_ov002_02085728(void *, void *);

void func_ov002_0208578c(u8 *a0)
{
    s32 i;
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 *q = p + 0x156;

    for (i = 0; i < 4; i++) {
        if (a0[i] != 0)
            func_ov002_02085728(data_ov002_020fb544 + a0[i] * 0x34, q);
        q += 0xc;
    }
}
