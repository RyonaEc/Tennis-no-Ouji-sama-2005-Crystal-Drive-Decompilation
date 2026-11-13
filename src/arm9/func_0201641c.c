#include "types.h"

extern u32 func_020176f0(u32);
extern u32 func_02017544(void);

typedef struct {
    u8 pad_0[2];
    u16 f0x2;
} S_0201641c;

u32 func_0201641c(u32 a0)
{
    u32 t0, t1;
    if (((S_0201641c *)a0)->f0x2 != 0x0) {
        t0 = func_020176f0(((S_0201641c *)a0)->f0x2);
        return t0;
    }
    t1 = func_02017544();
    return t1;
}
