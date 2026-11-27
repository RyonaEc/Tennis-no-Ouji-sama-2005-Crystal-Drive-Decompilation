#include "types.h"

extern u8 data_ov002_02102070[];

typedef struct {
    u8 b[12];
} B12;

u32 func_ov002_020855dc(u32 a0, u32 a1);

void func_ov002_020854dc(u32 id) {
    u8 *base = *(u8 **)data_ov002_02102070;
    u8 *ent = base + 0x2e4 + id * 0x158;
    u8 *e = base + 0x2e4;
    s32 i;
    u32 flag = 0;
    u8 *q = base;
    u8 *s = base + 0x186;
    u8 *o = base + id * 0xc;
    u8 *dst = base + 0x156 + id * 0xc;

    for (i = 0; i < 4; i++) {
        if (e[0] != 0 && ((u32)e[2] >> 1) != ((u32)ent[2] >> 1)) {
            if (flag == 0) {
                *(B12 *)(o + 0x156) = *(B12 *)(q + 0x186);
                flag++;
            } else {
                func_ov002_020855dc((u32)dst, (u32)s);
            }
        }
        q += 0xc;
        s += 0xc;
        e += 0x158;
    }
}