#include "types.h"

extern u32 func_02026350(void);
extern u32 func_02025024(u32, u32);
extern u32 func_02026364(u32);

u32 func_02025118(u32 a0)
{
    u32 t0, t1, t2;
    t0 = func_02026350();
    *(u32 *)((u8 *)(a0) + 100) = 0x1;
    t1 = func_02025024(t0, 0x1);
    t2 = func_02026364(t0);
    return t2;
}
