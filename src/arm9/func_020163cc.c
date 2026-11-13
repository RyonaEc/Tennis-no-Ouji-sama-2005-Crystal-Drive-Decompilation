#include "types.h"

extern u32 func_0202ef98(u32);
extern u32 func_020176f0(u32);
extern u32 func_02017710(u32);
extern u8 func_02016390[];

u32 func_020163cc(void)
{
    u32 t0, t1, t2, t3;
    t0 = func_0202ef98((u32)func_02016390);
    if (t0 != 0x2) {
        t1 = func_020176f0(t0);
        t2 = func_02017710(0x7);
        return 0x0;
    }
    t3 = func_02017710(0x3);
    return 0x1;
}
