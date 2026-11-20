#include "types.h"

extern u8 *data_ov003_020eb720;

s32 func_ov003_020540fc(void) {
    u8 *b;
    u8 *bp;
    u16 c;
    s32 i;
    s16 *p;
    s16 x, y, z, w;

    b = data_ov003_020eb720;
    i = *(u16 *)(b + 0x871e);
    bp = b + 0x899c;
    p = (s16 *)bp + i;
    x = *(s16 *)(bp + i * 2);
    y = p[2];
    c = *(u16 *)(b + 0x8720);
    if (c != 0) {
        z = p[4];
        w = p[6];
        *(u16 *)0x04000040 = ((x << 8) & 0xff00) | (y & 0xff);
        *(u16 *)0x04000044 = ((z << 8) & 0xff00) | (w & 0xff);
        *(u16 *)(b + 0x8720) = 0;
    }
    return 0;
}