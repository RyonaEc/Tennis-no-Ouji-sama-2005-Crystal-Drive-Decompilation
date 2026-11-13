#include "types.h"

extern u32 func_02018860(u32, u32, u32);
extern u8 data_02045650[];

u32 func_02018790(u32 a0, u32 a1)
{
    *(u32 *)((u8 *)data_02045650) = a0;
    *(u32 *)((u8 *)data_02045650 + 4) = a1;
    return func_02018860((u32)data_02045650, a1, (u32)data_02045650);
}
