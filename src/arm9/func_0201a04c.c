#include "types.h"

extern u8 data_02042360[];
extern u8 data_02046b34[];

u16 func_0201a04c(void)
{
    return ((u32)(*(u8 **)data_02042360) - (u32)data_02046b34) / 24;
}