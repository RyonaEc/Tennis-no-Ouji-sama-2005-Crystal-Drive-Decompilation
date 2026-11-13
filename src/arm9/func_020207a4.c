#include "types.h"

extern u8 data_02048c8c[];
extern u32 func_02020878(u8 *);
u32 func_020207a4(void)
{
    *(volatile u32 *)0x04001000 &= ~0x80000000;
    return func_02020878(data_02048c8c);
}
