#include "types.h"

extern u32 func_020272d4(u32, u32, u32, u32);
extern u32 func_0201c238(u32);
extern u32 func_020274c4(u32);

void func_0201c334(u32 a0, u32 a1, u32 a2, u32 a3)
{
    u32 t0 = func_020272d4(*(u8 *)((u8 *)a0 + 60), a1, a2, a3);
    u32 t1 = func_0201c238(a0);
    u32 t2 = func_020274c4(t1);
    *(u32 *)((u8 *)a0 + 48) = t2;
    *(u8 *)((u8 *)a0 + 44) = 0x1;
}
