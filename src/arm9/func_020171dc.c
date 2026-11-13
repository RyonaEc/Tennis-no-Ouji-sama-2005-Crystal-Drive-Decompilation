#include "types.h"

extern u32 func_02017710(u32);
extern u32 func_020170d0(u32, u32, u32, u32);
extern u8 data_020422f4[];
extern u8 data_02042b28[];

u32 func_020171dc(void)
{
    u32 t0 = func_02017710(0x3);
    *(u16 *)((u8 *)data_020422f4) = 0x1;
    *(u16 *)((u8 *)data_02042b28 + 8) = 0xffff;
    *(u16 *)((u8 *)data_02042b28 + 10) = 0xffff;
    return func_020170d0((u32)data_02042b28, (u32)data_020422f4, 0xffff, 0x1);
}
