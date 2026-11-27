#include "types.h"

extern u8 *data_ov002_02102070;
extern u8 data_ov002_020fb544[];

typedef struct { u32 a, b, c; } Blk;

typedef struct {
    u16 flags;
    u8 f2, f3, f4, f5, f6, f7;
    u8 pad8[8];
} Hdr;

typedef struct {
    Blk blk;
    u32 f0c, f10, f14;
} Pay;

typedef struct { u8 pad[8]; s32 f8; } Ent;

void func_ov002_0207679c(void);
u8 *func_ov002_02077974(void *a0, u32 a1, u32 a2, u32 a3, u32 a4);

u8 *func_ov002_02076a58(s32 a, u32 b, u32 c, u32 d) {
    Ent *tbl;
    Hdr *h;
    Pay *p;
    u8 *sb;

    sb = data_ov002_02102070 + 0x2e4 + a * 0x158;
    tbl = (Ent *)(data_ov002_020fb544 + *sb * 0x34);
    h = (Hdr *)func_ov002_02077974((void *)func_ov002_0207679c, 0x18, 1, 0, 0);
    if (h != 0) {
        p = (Pay *)(h + 1);
        h->flags = h->flags | 2;
        h->f5 = a;
        h->f7 = 0x23;
        p->blk = *(Blk *)(sb + 4);
        p->blk.c = p->blk.c + tbl->f8 / 2;
        p->f0c = b;
        p->f10 = c;
        p->f14 = d;
        p->f14 = p->f14 + 0x20000;
    }
    return (u8 *)h;
}