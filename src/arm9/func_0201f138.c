#include "types.h"

extern u32 func_02026350(void);
extern u32 func_020178e8(u32, u32);
extern u32 func_02026364(u32);
extern u8 data_02047f08[];

u32 func_0201f138(u32 a0)
{
    u32 t0 = func_02026350();
    u32 t1 = func_020178e8((u32)data_02047f08, a0);
    return func_02026364(t0);
}
