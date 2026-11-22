#include "types.h"

extern u32 data_0204b6e0;
extern void (*data_0204b6e4)(void);
extern void func_020263cc(void);
extern void func_0202ddc4(void);

void func_0202dde8(u32 a0, u32 a1)
{
    u32 (*cb)(void);
    u32 r;

    if ((a1 & 0x3f) == 0x11) {
        cb = (u32 (*)(void))data_0204b6e4;
        r = 1;
        data_0204b6e0 = 1;
        if (cb != 0)
            r = cb();
        if (r != 0)
            func_0202ddc4();
    } else {
        func_020263cc();
    }
}