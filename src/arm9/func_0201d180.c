#include "types.h"

extern u8 data_02047e98[];
extern u8 data_02047eb0[];

u32 func_0201d180(void)
{
    *(u32 *)(data_02047e98) = 0x0;
    *(u32 *)(data_02047eb0) = 0x0;
    return (u32)data_02047eb0;
}
