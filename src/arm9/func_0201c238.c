#include "types.h"

extern u32 func_0201fca8(u32, u32);
extern u32 func_0201fc78(u32, u32, u32);

typedef struct {
    u8 pad_0[45];
    u8 f0x2d;
    u8 f0x2e;
    u8 pad_47[5];
    u16 f0x34;
    u8 pad_54[8];
    u16 f0x3e;
    u8 f0x40;
    u8 f0x41;
} S_0201c238;

u32 func_0201c238(u32 a0)
{
    u32 t0, t1;
    ((S_0201c238 *)a0)->f0x2e = 0x0;
    ((S_0201c238 *)a0)->f0x2d = 0x0;
    ((S_0201c238 *)a0)->f0x34 = 0x0;
    ((S_0201c238 *)a0)->f0x3e = 0x0;
    ((S_0201c238 *)a0)->f0x40 = 0x7f;
    ((S_0201c238 *)a0)->f0x41 = 0x7f;
    t0 = func_0201fca8((a0 + 0x1c), 0x7f);
    t1 = func_0201fc78((a0 + 0x1c), 0x7f00, 0x1);
    return t1;
}
