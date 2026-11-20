#include "types.h"

extern u8 data_0203d06c[];
extern u8 data_02042a68[];
extern u32 func_0200fb38(void);
extern void func_0200fb20(u32);
extern void func_02012454(u32, u32);

void func_02010000(u32 a0)
{
    u8 *d;
    u32 v;

    if ((*(u8 *)(data_0203d06c + a0 * 0x18) & 4) != 0) a0 = a0 - 1;
    d = *(u8 **)(*(u8 **)(data_02042a68 + 0x10) + 0x154);
    *(u8 *)(d + a0 + 0x14) = 0;
    switch (a0)
    {
    case 0x3a:
        v = func_0200fb38();
        func_0200fb20((u16)(v & ~8));
        func_02012454(4, 0);
        func_02012454(3, 0);
        break;
    case 0x3b:
        func_02012454(6, 0);
        func_02012454(5, 0);
        break;
    }
}