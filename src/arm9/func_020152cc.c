#include "types.h"

extern u32 func_020153ec(void);
u32 func_020152cc(void)
{
    if (func_020153ec() == 0) return 0;
    *(volatile u16 *)0x04000204 |= 0x8000;
    return 1;
}
