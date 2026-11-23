#include "types.h"

extern u8 data_027c0000;

void func_02005418(void)
{
    *(u32 *)((u8 *)&data_027c0000 + 0x3ff8) |= 1;
}