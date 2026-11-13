#include "types.h"

extern u32 func_02027290(u32, u32);
extern u32 func_020272a8(u32);
extern u32 func_0201c034(u32);

typedef struct {
    u8 pad_0[44];
    u8 f0x2c;
    u8 pad_45[15];
    u8 f0x3c;
} S_0201c140;

u32 func_0201c140(u32 a0)
{
    u32 t0, t1, t2;
    if (((S_0201c140 *)a0)->f0x2c == 0x2) {
        t0 = func_02027290(((S_0201c140 *)a0)->f0x3c, 0xfffffd2d);
    }
    t1 = func_020272a8(((S_0201c140 *)a0)->f0x3c);
    t2 = func_0201c034(a0);
    return t2;
}
