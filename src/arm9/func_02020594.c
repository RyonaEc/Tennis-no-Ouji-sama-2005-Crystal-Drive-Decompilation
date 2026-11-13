#include "types.h"

extern u8 data_02048c8c[];
extern u32 func_020206e0(u8 *);
u32 func_02020594(void)
{
    *(volatile u32 *)0x04001000 &= ~0x80000000;
    return func_020206e0(data_02048c8c);
}
