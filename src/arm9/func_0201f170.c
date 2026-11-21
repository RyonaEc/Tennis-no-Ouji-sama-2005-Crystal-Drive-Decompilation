#include "types.h"

extern u8 data_02047f08[];
extern u32 func_02026350(void);
extern u32 func_02017784(void *p, u32 a);
extern void func_0201779c(void *p, u32 v);
extern void func_02026364(u32 s);

u32 func_0201f170(void)
{
    u32 s = func_02026350();
    u32 v = func_02017784(data_02047f08, 0);

    if (v)
        func_0201779c(data_02047f08, v);
    func_02026364(s);
    return v;
}