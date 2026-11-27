#include "types.h"

extern u8 *data_ov002_02102070;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

s32 func_02034148(s32 a0);

s32 func_ov002_020b357c(s32 id) {
    u8 *base = data_ov002_02102070;
    u8 *ent = base + id * 0x158;
    u8 *slot = base + 0x2e4;
    u8 tgt = *(u8 *)(ent + 0x2e6) / 2;
    Vec3 c[2];
    s32 n = 0;
    s32 r = 1;
    s32 i;
    s32 k;

    for (i = 0; i < 4; i++) {
        if (*(u8 *)(slot) != 0 && (*(u8 *)(slot + 2) / 2) != tgt) {
            c[n] = *(Vec3 *)(slot + 4);
            n++;
        }
        slot += 0x158;
    }
    if (n == 1) {
        if (c[0].x > 0)
            r = 0;
    } else {
        c[0].y = func_02034148(c[0].y);
        c[1].y = func_02034148(c[1].y);
        k = (c[0].y <= c[1].y) ? 1 : 0;
        if (c[k].x > 0)
            r = 0;
    }
    return r;
}