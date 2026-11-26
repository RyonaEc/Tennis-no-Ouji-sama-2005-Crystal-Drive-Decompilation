#include "types.h"

extern u32 data_02047dc8;
extern u8 data_02047dcc[];
extern void func_02017994(u8 *p, u16 v);
extern void func_0201c8c0(void);
extern void func_0201c848(void);

void func_0201ce4c(u8 *p)
{
    if (data_02047dc8 == 0) {
        func_02017994(data_02047dcc, 0);
        data_02047dc8 = 1;
    }
    *(u32 *)(p + 8) = (u32)func_0201c8c0;
    *(u32 *)(p + 0xc) = (u32)p;
    *(u32 *)(p + 0x14) = (u32)func_0201c848;
    *(u32 *)(p + 0x18) = (u32)p;
    *(u32 *)(p + 0x44) = 0;
    *(u32 *)(p + 0x48) = 0;
    *(u32 *)(p + 0x24) &= ~1;
    *(u32 *)(p + 0x24) &= ~2;
}