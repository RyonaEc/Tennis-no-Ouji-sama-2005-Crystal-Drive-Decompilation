#include "types.h"

extern u16 data_02042b40;
extern void func_02025c94(u16 *, u32);
extern u32 func_02015fa0(void);

u32 func_02015fe8(void)
{
    if (data_02042b40 & 0x8000)
        return 1;
    data_02042b40 |= 0x8000;
    func_02025c94(&data_02042b40, 0x18);
    return func_02015fa0();
}