#include "types.h"

extern u8 data_0204abe8[];

extern s32 func_02029704(void);
extern u32 func_02026350(void);
extern void func_02026364(u32);

u8 *func_0202963c(void)
{
    s32 v = func_02029704();
    u32 t;
    u8 *p;

    t = func_02026350();
    p = *(u8 **)data_0204abe8;
    while (p != 0 && *(s32 *)p != v)
        p = *(u8 **)(p + 4);
    func_02026364(t);
    return p;
}