#include "types.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
    u8 pad14[0x48];
    s32 f5c;
    s32 f60;
    s32 f64;
    u8 pad68[8];
    s32 f70;
    u8 pad74[0x44];
    s32 fb8;
    u8 padbc[0x10];
    s32 fcc;
} Obj;

s32 func_02034148(s32 a0);

void func_ov002_020e6a5c(Obj *o) {
    s32 v;

    o->fb8 = o->fb8 | 8;
    o->fcc = 0x12b;
    v = func_02034148(o->f5c) / 3;
    if (v < 0x8000)
        v = 0x8000;
    else if (v > 0x20000)
        v = 0x20000;
    if (o->f0 < 0)
        o->f5c = v;
    else
        o->f5c = -v;
    if (o->fb8 & 2)
        o->f60 = -0x20000;
    else
        o->f60 = 0x20000;
    o->f64 = 0x45555;
    o->f70 = -0x1194;
    if (o->f4 >= 0 && o->f10 <= 0) {
        if (o->f4 < 0x10000)
            o->f4 = 0x10000;
    } else {
        if (o->f4 > -0x10000)
            o->f4 = -0x10000;
    }
}