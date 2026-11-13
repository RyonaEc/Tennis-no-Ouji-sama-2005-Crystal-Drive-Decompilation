#include "types.h"

extern u8 data_02042b00[];

u32 func_020160cc(void)
{
    return *(u32 *)((u8 *)data_02042b00);
}
