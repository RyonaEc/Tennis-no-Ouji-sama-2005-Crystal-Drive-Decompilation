#include "types.h"

typedef struct {
    u8 pad0[0x10];
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
    u32 f20;
    u32 f24;
    u32 f28;
} Obj;

extern u8 data_020475c4[];
extern void func_0201acec(u8 *p);

void func_0201abfc(Obj *o, u32 a1)
{
    u16 i = 0;
    u8 *r4 = *(u8 **)data_020475c4 + 0x28;
    u8 *e;

    for (i = 0; i < *(u16 *)(a1 + 0); i++) {
        if (o->f20 != 0)
            ((void (*)(u8 *, u32, u32))o->f20)(*(u8 **)data_020475c4, a1, i);
        if (*(u32 *)(*(u8 **)data_020475c4 + 0x20) != 0) {
            e = *(u8 **)(a1 + 4) + i * 6;
            *(u16 *)(r4 + 0) = *(u16 *)(e + 0);
            *(u16 *)(r4 + 2) = *(u16 *)(e + 2);
            *(u16 *)(r4 + 4) = *(u16 *)(e + 4);
            if (o->f28 != 0)
                ((void (*)(u8 *, u8 *, u32))o->f28)(*(u8 **)data_020475c4, r4, i);
            func_0201acec(r4);
        }
        if (o->f24 != 0)
            ((void (*)(u8 *, u32, u32))o->f24)(*(u8 **)data_020475c4, a1, i);
    }
}