#include "types.h"

extern u8 data_02048d34[];
extern u8 data_02048d50[];
extern void func_02026350(void);
extern void func_02025ac4(u32);
extern void func_02025508(u32);
extern void func_0202514c(u32);
extern void func_02025024(void);

void func_02025228(void)
{
    u8 *p;

    func_02026350();
    p = *(u8 **)(*(u32 *)data_02048d34);
    func_02025ac4((u32)p);
    func_02025508((u32)p);
    ((u32 *)data_02048d50)[*(u32 *)(p + 0x6c)] = 0;
    *(u32 *)(p + 0x64) = 2;
    func_0202514c((u32)(p + 0x90));
    func_02025024();
}