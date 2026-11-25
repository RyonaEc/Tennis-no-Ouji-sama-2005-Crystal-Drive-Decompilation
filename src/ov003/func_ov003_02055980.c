#include "types.h"

typedef struct {
    u8 pad0[0x29c];
    u16 f29c;
    u16 f29e;
    u16 f2a0;
    u16 f2a2;
    u8 pad2a4[0x18];
    u16 arr[0x4a];
    u16 f350;
    u16 f352;
} T_02055980;

extern u32 data_ov003_020eb720;
extern u32 func_02008c44(void);
extern u32 func_02008164(void);
extern void func_02008104(u16 *a0, u16 *a1, u16 *a2, u16 *a3);

void func_ov003_02055980(void)
{
    T_02055980 *p = (T_02055980 *)data_ov003_020eb720;
    u32 v;
    u16 idx;

    if (func_02008c44() != 0)
        p->f350 = 0;
    else
        p->f350 = 1;
    v = func_02008164();
    if (v == 0)
        p->f352 = 0;
    else if (v == 1)
        p->f352 = 1;
    else if (v == 2)
        p->f352 = 2;
    else if (v == 3)
        p->f352 = 3;
    else if (v == 4)
        p->f352 = 4;
    func_02008104(&p->f29c, &p->f29e, &p->f2a0, &p->f2a2);
    if (p->f29c != 0) {
        idx = p->f29c - 1;
        p->arr[idx] = 0;
    }
    if (p->f29e != 0) {
        idx = p->f29e - 1;
        p->arr[idx] = 1;
    }
    if (p->f2a0 != 0) {
        idx = p->f2a0 - 1;
        p->arr[idx] = 2;
    }
    if (p->f2a2 != 0) {
        idx = p->f2a2 - 1;
        p->arr[idx] = 3;
    }
}