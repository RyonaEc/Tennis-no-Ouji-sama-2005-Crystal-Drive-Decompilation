#include "types.h"

extern u8 data_020475fc[];
extern u8 data_020475f0[];
extern u8 data_02047988[];
extern u8 data_02047608[];
extern void func_02017994(u8 *, u32);
extern void func_020178e8(u8 *, u8 *);

void func_0201c7a0(void)
{
    s32 i;
    u8 *p;

    func_02017994(data_020475fc, 0x14);
    func_02017994(data_020475f0, 0x14);
    p = data_02047988;
    for (i = 0; i < 0x10; i++, p += 0x44)
    {
        p[0x2c] = 0;
        p[0x3c] = i;
        func_020178e8(data_020475f0, p);
    }
    p = data_02047608;
    for (i = 0; i < 0x20; i++, p += 0x1c)
    {
        func_02017994(p, 0xc);
        func_02017994(p + 0xc, 0);
        *(u32 *)(p + 0x18) = 1;
    }
}