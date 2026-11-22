#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32);
extern void func_0202514c(u8 *);
extern void func_02025118(u8 *);

void func_0202d4f0(u8 *p)
{
    void (*cb)(u32) = *(void (**)(u32))(p + 0x28);
    u32 arg = *(u32 *)(p + 0x2c);
    u32 h = func_02026350();
    u32 *f = (u32 *)(p + 0x34);

    *f = *f & ~0xc;
    func_0202514c(p + 0xd4);
    if (*(u32 *)(p + 0x34) & 0x10)
        func_02025118(p + 0x3c);
    func_02026364(h);
    if (cb != 0)
        cb(arg);
}