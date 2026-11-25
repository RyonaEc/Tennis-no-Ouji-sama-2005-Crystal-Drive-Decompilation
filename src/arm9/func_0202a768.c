#include "types.h"

typedef struct {
    u8 pad0[0x14];
    u32 f14;
    u32 f18;
    s32 f1c;
    u8 buf[0x38];
    u8 f58[8];
} T_0202a768;

extern void func_0202a8bc(T_0202a768 *p);

void func_0202a768(T_0202a768 *p)
{
    if (p->f1c > 0x37) {
        p->buf[p->f1c++] = 0x80;
        while (p->f1c < 0x40)
            p->buf[p->f1c++] = 0;
        func_0202a8bc(p);
        while (p->f1c < 0x38)
            p->buf[p->f1c++] = 0;
    } else {
        p->buf[p->f1c++] = 0x80;
        while (p->f1c < 0x38)
            p->buf[p->f1c++] = 0;
    }
    p->f58[0] = p->f18 >> 24;
    p->f58[1] = p->f18 >> 16;
    p->f58[2] = p->f18 >> 8;
    p->f58[3] = p->f18;
    p->f58[4] = p->f14 >> 24;
    p->f58[5] = p->f14 >> 16;
    p->f58[6] = p->f14 >> 8;
    p->f58[7] = p->f14;
    func_0202a8bc(p);
}