#include "types.h"

extern void func_02030828(void);
extern void func_02030888(void);

extern u8 data_0204c188[];
extern u8 data_0204c18c[];
extern void func_020307c8(void);
extern void func_02027d68(void);
extern u32 func_02027f4c(u32, u32);
extern void func_02027f74(u32, u32);
extern void func_020308c4(void);

void func_02030a80(void)
{
    if (*(u32 *)data_0204c188 != 0) return;
    *(u32 *)data_0204c188 = 1;
    func_020307c8();
    func_02027d68();
    while (func_02027f4c(0xd, 1) == 0) {}
    func_02027f74(0xd, (u32)func_02030888);
    func_020308c4();
    func_02027f74(0xd, 0);
    func_02027f74(0xd, (u32)func_02030828);
    *(u32 *)data_0204c18c = 0;
}