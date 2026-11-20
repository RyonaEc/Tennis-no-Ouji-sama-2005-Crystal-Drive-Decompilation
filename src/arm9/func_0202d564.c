#include "types.h"

extern u8 data_0204aec0[];
extern u8 data_0204b4c0[];
extern void func_0202d738(void);
extern void func_0202ccec(void);
extern void func_0202de54(void);

void func_0202d564(void)
{
    u8 *d = data_0204aec0;

    if (*(u32 *)(d + 0x34) != 0) return;
    *(u32 *)(d + 0x34) = 1;
    *(u32 *)(d + 0x20) = 0;
    *(u32 *)(d + 0x1c) = *(u32 *)(d + 0x20);
    *(u32 *)(d + 0x18) = *(u32 *)(d + 0x1c);
    *(u32 *)(d + 0x24) = 0xffffffff;
    *(u32 *)(d + 0x28) = 0;
    *(u32 *)(d + 0x2c) = 0;
    func_0202ccec();
    *(u32 *)data_0204b4c0 = (u32)func_0202d738;
    func_0202de54();
}