#include "types.h"

typedef struct S_node S_node;

struct S_node {
    u8 pad0[4];
    S_node *f4;
    u8 pad8[0x10];
    void *f18;
};

typedef struct {
    u8 pad0[0x18];
    struct {
        S_node *node;
        void *arg;
    } slot[4];
} S_02027c04;

extern void func_02025c78(u32 a0, u32 a1);

void func_02027c04(S_02027c04 *p)
{
    s32 i;
    S_node *n;
    S_node *x;

    for (i = 0; i < 4; i++) {
        n = p->slot[i].node;
        if (n == 0)
            continue;
        if (&p->slot[i] == n->f18) {
            n->f18 = p->slot[i].arg;
            func_02025c78((u32)n, 0x3c);
        } else {
            x = n->f18;
            if (x != 0) {
                do {
                    if ((S_node *)&p->slot[i] == x->f4)
                        break;
                    x = x->f4;
                } while (x != 0);
            }
            x->f4 = p->slot[i].arg;
            func_02025c78((u32)x, 8);
        }
    }
}