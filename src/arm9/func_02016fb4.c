#include "types.h"

extern u32 func_0201610c(void);
extern u32 func_0202e674(u32);
extern u32 func_0202ee48(u32, u32);
extern u32 func_020176f0(u32);
extern u32 func_02017710(u32);
extern u8 data_02042300[];
extern u8 func_02016f6c[];

u32 func_02016fb4(void)
{
    u32 t0, t1, t2, t3, t4;
    t0 = func_0201610c();
    t1 = func_0202e674(t0);
    *(u16 *)(data_02042300 + 24) = t1;
    t2 = func_0202ee48((u32)func_02016f6c, (u32)data_02042300);
    if (t2 != 0x2) {
        t3 = func_020176f0(t2);
        return 0x0;
    }
    t4 = func_02017710(0x3);
    return 0x1;
}
