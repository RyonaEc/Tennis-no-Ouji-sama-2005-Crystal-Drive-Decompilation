#include "types.h"

typedef struct {
    u8 pad0[4];
    u32 f4;
    u8 pad8[0x10];
    u32 f18;
} N_02027c9c;

typedef struct {
    u32 head;
    u32 tail;
} B_02027c9c;

typedef struct {
    u32 pad0[6];
    B_02027c9c b[64];
} T_02027c9c;

extern void func_02025c78(u32 a0, u32 a1);

void func_02027c9c(T_02027c9c *a0, u32 a1, N_02027c9c *node)
{
    N_02027c9c *cur;
    N_02027c9c *p;
    u32 t;

    cur = (N_02027c9c *)a0->b[a1].head;
    if (cur != 0) {
        if (node == cur)
            return;
        if ((N_02027c9c *)&a0->b[a1] == (N_02027c9c *)cur->f18) {
            cur->f18 = a0->b[a1].tail;
            func_02025c78(a0->b[a1].head, 0x3c);
        } else {
            p = (N_02027c9c *)cur->f18;
            while (p != 0) {
                t = p->f4;
                if ((N_02027c9c *)&a0->b[a1] == (N_02027c9c *)t)
                    break;
                p = (N_02027c9c *)t;
            }
            p->f4 = a0->b[a1].tail;
            func_02025c78((u32)p, 8);
        }
    }
    t = node->f18;
    node->f18 = (u32)&a0->b[a1];
    a0->b[a1].tail = t;
    a0->b[a1].head = (u32)node;
    func_02025c78((u32)a0, 0x3c);
    func_02025c78((u32)node, 0x3c);
}