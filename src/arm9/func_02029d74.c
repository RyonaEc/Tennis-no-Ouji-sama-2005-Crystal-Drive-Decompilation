#include "types.h"

extern u32 func_02028db8(u32, u32, u32, u32);
extern u8 data_0204ac14[];

u32 func_02029d74(u32 a0, u32 a1)
{
    return func_02028db8((u32)data_0204ac14, a0, a1, a0);
}
