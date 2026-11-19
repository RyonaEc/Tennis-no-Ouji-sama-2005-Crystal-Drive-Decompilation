#include "types.h"

extern u32 data_02049034;
extern u32 data_02049038;
extern u32 data_02049050;

u32 func_020274c4(void)
{
    if (data_02049038 == 0)
        return data_02049034;

    return data_02049050;
}