#include "types.h"

extern u8 data_02048c8a[];
extern u32 func_020206e0(u8 *);
u32 func_020205bc(void)
{
    *(volatile u32 *)0x04001000 &= ~0x40000000;
    return func_020206e0(data_02048c8a);
}
