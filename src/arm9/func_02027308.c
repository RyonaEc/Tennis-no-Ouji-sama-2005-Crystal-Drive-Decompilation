#include "types.h"

extern u32 data_02049030;
extern u32 data_02049040;

u32 *func_02027308(void)
{
    u32 *n = (u32 *)data_02049030;

    if (n == 0)
        return 0;
    data_02049030 = *n;
    if (data_02049030 == 0)
        data_02049040 = 0;
    return n;
}