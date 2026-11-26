#include "types.h"

typedef struct {
    u8  pad0[0x10];
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
    u32 f20;
} Obj;

extern u8 data_020475c4[];
extern void func_0201abfc(Obj *o, u32 a0);
extern void func_0201b1e4(Obj *o, u32 a0);

void func_0201a98c(u32 a0)
{
    u8 *p = *(u8 **)data_020475c4;
    Obj *o = *(Obj **)p;

    if (o->f10 == 0)
        return;
    if (o->f18 != 0)
        ((void (*)(u8 *, u32))o->f18)(p, a0);
    if (*(u32 *)(*(u8 **)data_020475c4 + 0x20) != 0) {
        switch (o->f14) {
        case 0:
            func_0201abfc(o, a0);
            break;
        case 1:
        case 2:
            func_0201b1e4(o, a0);
            break;
        case 3:
            break;
        }
    }
    if (o->f1c != 0)
        ((void (*)(u8 *, u32))o->f1c)(*(u8 **)data_020475c4, a0);
}