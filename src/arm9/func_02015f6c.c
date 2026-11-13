#include "types.h"

extern u32 func_020176f0(u32);
extern void func_02017710(u32);
void func_02015f6c(u8 *p)
{
    u32 v = *(u16 *)(p + 2);
    if (v == 0) return;
    func_020176f0(v);
    func_02017710(7);
}
