#include "types.h"

extern u8 data_02042b40[];
extern u8 data_02042b44[];
extern u8 data_02042ae8[];
extern void func_02026564(u8 *);
extern void func_020269d0(u8 *, u8 *, u32);
extern u32 func_02015574(void);

void func_0201610c(void)
{
    u8 buf[0x54];

    func_02026564(buf);
    *(u16 *)(data_02042b40 + 2) = *(u16 *)(buf + 0x18);
    func_020269d0(buf + 4, data_02042b44, 0x14);
    *(u16 *)data_02042b40 = *(u16 *)data_02042ae8 & 7;
    if (func_02015574() != 0)
        *(u16 *)data_02042b40 = *(u16 *)data_02042b40 | 0x4000;
}