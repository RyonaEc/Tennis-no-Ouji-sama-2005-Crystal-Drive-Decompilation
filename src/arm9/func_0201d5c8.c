#include "types.h"

extern u32 data_02047f00;

u32 func_0201d5c8(u32 v)
{
    u32 old = data_02047f00;

    data_02047f00 = v;
    return old;
}