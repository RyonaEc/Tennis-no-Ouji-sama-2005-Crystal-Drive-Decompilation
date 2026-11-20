#include "types.h"

extern u8 data_0204236c[];
extern u8 data_02048c70[];

void func_02020300(void)
{
    u32 v = *(u32 *)0x04000000;
    u16 m = (u16)((v & 0x30000) >> 16);
    *(u16 *)data_0204236c = 0;
    *(u16 *)data_02048c70 = m;
    *(u32 *)0x04000000 = v & ~0x30000;
}