#include "types.h"

extern u8 data_02047f14[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern u32 func_02017784(u8 *a, u8 *b);
extern u32 func_0201779c(u32 a, u32 b);

u8 *func_0201f1bc(void)
{
    u32 s = func_02026350();
    u8 *p = (u8 *)func_02017784(data_02047f14, 0);

    if (p != 0) {
        u32 q;

        func_0201779c((u32)data_02047f14, (u32)p);
        q = *(u32 *)(p + 8);
        *(u32 *)(q + 0x114) -= 1;
    }
    func_02026364(s);
    return p;
}