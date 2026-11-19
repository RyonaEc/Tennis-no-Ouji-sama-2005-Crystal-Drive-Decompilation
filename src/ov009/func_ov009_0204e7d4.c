#include "types.h"

extern u32 data_ov009_0205331c[];

void func_ov009_0204e7d4(u32 i)
{
    *(u32 *)0x04000484 = data_ov009_0205331c[i];
}