#include "types.h"

extern u32 func_020176f0(u32);
extern void func_02017544(u32);
void func_02016b4c(u8 *p)
{
    u32 v = *(u16 *)(p + 2);
    if (v == 0) return;
    func_02017544(func_020176f0(v));
}
