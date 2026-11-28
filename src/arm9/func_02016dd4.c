#include "types.h"

extern u16 data_02042af4;
extern u8 data_02042c14[];
extern u32 func_020176f0(u32 a0);
extern u32 func_02017544(void);
extern u32 func_02015574(void);
extern void func_02015558(void);
extern void func_02026a50(u32 a0, u32 a1, u32 a2);
extern void func_02025c94(u32 a0, u32 a1);
extern u32 func_02015614(void);
extern u32 func_02015630(void);
extern u32 func_02017710(u32 a0);
extern s32 func_02016d1c(void);

void func_02016dd4(u16 *h)
{
    u32 v = h[1];

    if (v != 0) {
        func_020176f0(v);
        func_02017544();
        return;
    }
    v = h[4];
    if (v == 2)
        return;
    if (v == 7) {
        if (func_02015574() != 0)
            func_02015558();
        data_02042af4 |= 1 << h[8];
        func_02026a50((u32)(h + 0xa), (u32)(data_02042c14 + h[8] * 0x18), 0x18);
        func_02025c94((u32)(data_02042c14 + h[8] * 0x18), 0x18);
        return;
    }
    if (v == 9) {
        data_02042af4 &= ~(1 << h[8]);
        if (func_02015614() != 0)
            return;
        if (func_02015630() == 0)
            return;
        func_020176f0(0x18);
        func_02017710(8);
        return;
    }
    if (v != 0)
        return;
    func_02016d1c();
}