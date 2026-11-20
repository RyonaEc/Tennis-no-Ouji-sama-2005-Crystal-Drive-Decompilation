#include "types.h"

extern u8 data_02042a68[];
extern u32 func_02017b60(u32, u32, u32);
extern void func_02026a04(s32, u32, u32);

void func_0200eac4(void)
{
    volatile s32 z1;
    volatile s32 z2;
    u32 p;

    p = func_02017b60(*(u32 *)data_02042a68, 0x160, 4);
    z1 = 0;
    *(u32 *)(data_02042a68 + 0x10) = p;
    func_02026a04(z1, p, 0x160);
    p = func_02017b60(*(u32 *)data_02042a68, 0x5c, 4);
    *(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x150) = p;
    z2 = 0;
    func_02026a04(z2, *(u32 *)(*(u32 *)(data_02042a68 + 0x10) + 0x150), 0x5c);
}