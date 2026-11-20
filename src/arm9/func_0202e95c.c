#include "types.h"

extern u32 func_0202e2ec(void);
extern u32 func_0202e2d0(void);
extern void func_02025c5c(u32, u32);
extern void func_02026a50(u32, u32, u32);

u32 func_0202e95c(u32 a0)
{
    u32 d = func_0202e2ec();
    u32 r = func_0202e2d0();

    if (r != 0) return r;
    if (a0 == 0) return 6;
    func_02025c5c(*(u32 *)(d + 4), 0x930);
    func_02026a50(*(u32 *)(d + 4), a0, 0x930);
    return 0;
}