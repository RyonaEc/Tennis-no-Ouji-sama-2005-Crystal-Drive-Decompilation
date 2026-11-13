#include "types.h"

extern u8 data_02042798[];

u32 func_020343dc(u32 a0, u32 a1, u32 a2, u32 a3)
{
    return ((u32 (*)(u32, u32, u32, u32))*(u32 *)((u8 *)(*(u32 *)(data_02042798 + 8)) + 4))(a0, a1, *(u32 *)((u8 *)(*(u32 *)(data_02042798 + 8)) + 4), a3);
}
