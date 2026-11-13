#include "types.h"

extern u32 func_02015858(void);
extern u8 data_02042af0[];

u32 func_020160f0(void)
{
    *(u16 *)((u8 *)data_02042af0) = 0x0;
    return func_02015858();
}
