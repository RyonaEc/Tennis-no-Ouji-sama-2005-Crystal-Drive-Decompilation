#include "types.h"

extern u8 data_ov008_0204f2d4[];
extern u8 data_ov008_0204f440[];
extern s32 func_020101d0(u32 a0);

typedef struct { u32 w[9]; } Blk;

void func_ov008_0204dd80(void)
{
    Blk buf;
    u8 *p;
    u8 *q;
    s32 n;
    s32 i;

    buf = *(Blk *)data_ov008_0204f2d4;
    p = *(u8 **)data_ov008_0204f440;
    q = p + 0xf;
    n = 0;
    for (i = 0; i < 9; i++) {
        if (func_020101d0(buf.w[i])) {
            *q = i;
            q[1] = 0;
            n++;
            q += 2;
        }
    }
    {
        s8 *t = (s8 *)p;

        t[0xb] = n;
        t[0xc] = 0;
        t[0xd] = 0;
        t[0xe] = -1;
    }
}
