#include "types.h"

extern u32 func_02026350(void);
extern void func_02026364(u32 a);
extern s32 func_02027e98(u32 a, u32 b, u32 c);

extern struct TPSt data_0204ac68;

struct TPSt {
    s32 (*cb)(u32, u32, u32);
    u8 pad[0x30];
    volatile u16 f34;
    volatile u16 f36;
};

void func_0202b39c(void)
{
    u32 irq;
    s32 ok;

    irq = func_02026350();

    ok = func_02027e98(6, 0x03000000, 0) >= 0;

    if (!ok) {
        func_02026364(irq);

        data_0204ac68.f34 |= 1;

        if (data_0204ac68.cb != 0)
            data_0204ac68.cb(0, 4, 0);
        return;
    }

    data_0204ac68.f36 |= 1;
    data_0204ac68.f34 &= ~1;

    func_02026364(irq);
}