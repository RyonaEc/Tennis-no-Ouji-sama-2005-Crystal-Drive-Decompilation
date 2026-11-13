#include "types.h"

extern u32 func_02017710(u32);
extern u32 func_020176f0(u32);

typedef struct {
    u8 pad_0[2];
    u16 f0x2;
} S_02016390;

u32 func_02016390(u32 a0)
{
    u32 t0, t1, t2;
    if (((S_02016390 *)a0)->f0x2 != 0x0) {
        t0 = func_02017710(0x7);
        t1 = func_020176f0(((S_02016390 *)a0)->f0x2);
        return t1;
    }
    t2 = func_02017710(0x1);
    return t2;
}
