#include "types.h"

extern u8 data_02048d3c[];
extern u32 func_02026350(void);
extern void func_02025024(u32);
extern void func_02026364(u32);

void func_0202514c(u8 *a0)
{
    u32 v;
    u32 r;
    u32 *p;

    r = func_02026350();
    v = *(u16 *)a0;
    if (v != 0)
    {
        p = *(u32 **)(data_02048d3c + 0xc);
        while (p != 0)
        {
            if ((v & (1u << p[0x1b])) != 0) p[0x19] = 1;
            p = (u32 *)p[0x1a];
        }
        *(u16 *)a0 = 0;
        func_02025024(0);
    }
    func_02026364(r);
}