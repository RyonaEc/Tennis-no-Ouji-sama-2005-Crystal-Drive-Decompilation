#include "types.h"

extern u8 data_0204acb8[];
extern u32 func_02026350(void);
extern void func_02026364(u32);

u32 func_0202c3e0(void)
{
    u32 s = func_02026350();

    if (*(u32 *)data_0204acb8 != 0)
    {
        func_02026364(s);
        return 0;
    }
    *(u32 *)data_0204acb8 = 1;
    func_02026364(s);
    return 1;
}