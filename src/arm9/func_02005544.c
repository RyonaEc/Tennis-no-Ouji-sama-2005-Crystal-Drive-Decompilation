#include "types.h"

extern u8 data_02042a68[];
extern void func_02005bf4(s32);

void func_02005544(void)
{
    u8 *p;

    func_02005bf4(1);
    p = *(u8 **)(data_02042a68 + 0x10);
    *(u32 *)p |= 0x10;
}
