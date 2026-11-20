#include "types.h"

extern u8 data_02047f34[];
extern void func_02025b88(u8 *);
extern void func_02025b14(u8 *);
extern void func_0201cb54(u8 *);
extern void func_0201f2b0(u8 *);

void func_0201f38c(u8 *o)
{
    func_02025b88(data_02047f34);
    if ((s32)(*(u32 *)(o + 0x108) << 30) >> 31)
        func_0201cb54(o);
    func_0201f2b0(o);
    func_02025b14(data_02047f34);
}