#include "types.h"

extern u8 data_02042af8[];

u32 func_02016094(void)
{
    return *(u16 *)((u8 *)data_02042af8);
}
