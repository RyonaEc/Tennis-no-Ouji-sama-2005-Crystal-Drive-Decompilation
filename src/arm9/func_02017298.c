#include "types.h"

extern u32 func_020176f0(u32);
extern u32 func_0201725c(void);

typedef struct {
    u8 pad_0[2];
    u16 f0x2;
} S_02017298;

u32 func_02017298(u32 a0)
{
    u32 t0, t1;
    if (((S_02017298 *)a0)->f0x2 != 0x0) {
        t0 = func_020176f0(((S_02017298 *)a0)->f0x2);
        return t0;
    }
    t1 = func_0201725c();
    return t1;
}
