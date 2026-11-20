#include "types.h"

extern u8 *func_02018248(u32, u32, u32);
extern void func_0201d8f0(void);
extern void func_020178e8(u8 *, u8 *);

u32 func_0201d860(u8 *a0)
{
    u8 *p = func_02018248(*(u32 *)a0, 0x14, 4);
    if (p == 0) return 0;
    func_0201d8f0();
    func_020178e8(a0 + 4, p);
    return 1;
}