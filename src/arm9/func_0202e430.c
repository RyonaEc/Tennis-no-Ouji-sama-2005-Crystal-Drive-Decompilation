#include "types.h"

extern u8 data_0204b6e8[];
extern u8 data_0204b6ec[];
extern u32 func_0202e2d0(void);
extern u32 func_0202e1cc(u32, u32);
extern void func_0202de8c(void);
extern void func_02027f74(u32, u32);

u32 func_0202e430(void)
{
    u32 r;

    if (func_0202e2d0() != 0) return 3;
    r = func_0202e1cc(1, 0);
    if (r != 0) return r;
    func_0202de8c();
    func_02027f74(0xa, 0);
    *(u32 *)data_0204b6ec = 0;
    *(u16 *)data_0204b6e8 = 0;
    return 0;
}