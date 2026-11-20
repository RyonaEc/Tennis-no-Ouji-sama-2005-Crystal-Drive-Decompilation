#include "types.h"

extern u8 data_02041f70[];
extern u8 data_02041fa4[];
extern void func_02032ecc(u32, u32, u32, u32);
extern void func_02033cb0(void);

void func_02032f38(u32 a0, u32 a1, u32 a2, u32 a3)
{
    if (a3 != 0)
    {
        func_02032ecc((u32)data_02041f70, a0, a1, a2);
        func_02033cb0();
        for (;;)
            ;
    }
    func_02032ecc((u32)data_02041fa4, a0, a1, a2);
}