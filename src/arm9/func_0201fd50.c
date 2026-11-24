#include "types.h"

#define reg_CP_DIVCNT     (*(volatile u16 *)0x04000280)
#define reg_CP_DIV_NUMER  (*(volatile u64 *)0x04000290)
#define reg_CP_DIV_DENOM  (*(volatile u64 *)0x04000298)
#define FX32_ONE 0x1000

void func_0201fd50(s32 denom)
{
    reg_CP_DIVCNT    = 1;
    reg_CP_DIV_NUMER = (u64) FX32_ONE << 32;
    reg_CP_DIV_DENOM = (u32) denom;
}