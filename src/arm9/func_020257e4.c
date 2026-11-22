#include "types.h"

extern u32 data_020423dc;
extern u32 func_02025710(void);
extern u32 func_0202585c(void);
extern u32 func_020256dc(void);

u32 func_020257e4(void)
{
    u32 v;

    if (data_020423dc == (u32)-1) {
        v = func_02025710();
        if (func_0202585c() != 0)
            v |= 0x10000000;
        else if (func_020256dc() != 0)
            v |= 0x40000000;
        else if (v & 0x1000000)
            v |= 0x20000000;
        else
            v |= 0x80000000;
        data_020423dc = v | *(u16 *)0x027ffffa;
    }
    return data_020423dc;
}