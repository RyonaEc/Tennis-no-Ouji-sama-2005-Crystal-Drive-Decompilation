#include "types.h"

typedef struct {
    u8 pad0[0x20];
    u32 f20;
    u32 f24;
} Q;

typedef struct {
    u8 pad0[8];
    Q *f8;
    u8 pad1[0x2c - 0xc];
    u32 f2c;
    u32 f30;
    u32 f34;
} P;

typedef struct {
    Q *container;
    u8 *item;
} Pair;

extern void func_02028a38(Pair *, u32 *, u32);
extern u32 func_02028af4(P *, u32);

u32 func_0202811c(P *p)
{
    Q *q;
    u32 idx;
    u32 out[2];
    Pair in;

    q = (Q *)p->f8;
    idx = p->f30;
    if (idx * 8 >= q->f24)
        return 1;
    in.container = q;
    in.item = (u8 *)q->f20 + idx * 8;
    func_02028a38(&in, out, 8);
    p->f2c = out[0];
    p->f30 = out[1];
    p->f34 = idx;
    return func_02028af4(p, 7);
}