#include "types.h"

extern u8 data_02047f04[];
extern u8 data_02047f20[];
extern u8 data_02047f34[];
extern void func_020259c0(u8 *, u8 *, u32);
extern void func_02025864(u8 *, u32 *, u32);
extern void func_02025b88(u8 *);
extern u32 func_0201f1bc(void);
extern void func_02025b14(u8 *);
extern void func_0201e7dc(void);
extern void func_0201f138(u32);

void func_0201e760(void)
{
    u32 v;
    u32 r;

    func_020259c0(data_02047f20, data_02047f04, 1);
    for (;;)
    {
        func_02025864(data_02047f20, &v, 1);
        for (;;)
        {
            func_02025b88(data_02047f34);
            r = func_0201f1bc();
            if (r == 0)
            {
                func_02025b14(data_02047f34);
                break;
            }
            func_0201e7dc();
            func_0201f138(r);
            func_02025b14(data_02047f34);
        }
    }
}