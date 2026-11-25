#include "types.h"

extern u32 func_020251c0(u16 *a0);
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_02028c98(u8 *a0, u32 a1);
extern void func_02029d00(u8 *a0, u8 *a1);
extern void func_020293a4(u8 *a0);
extern u32 func_0202935c(u8 *a0);

s32 func_020291f4(u8 *a0, u32 a1)
{
    u8 *h;
    u32 bit;
    u32 r4;
    u32 b;

    h = *(u8 **)(a0 + 8);
    *(u32 *)(a0 + 0x10) = a1;
    *(u32 *)(a0 + 0x14) = 2;
    bit = 1 << a1;
    *(u32 *)(a0 + 0xc) = *(u32 *)(a0 + 0xc) | 1;
    r4 = func_02026350();
    if (*(u32 *)(h + 0x10) & 0x80) {
        func_02028c98(a0, 3);
        func_02026364(r4);
        return 0;
    }
    if (bit & 0x1fc)
        *(u32 *)(a0 + 0xc) = *(u32 *)(a0 + 0xc) | 4;
    func_02029d00(a0, h + 0x14);
    b = (*(u32 *)(h + 0x10) & 0x10) != 0;
    if (b == 0) {
        *(u32 *)(h + 0x10) = *(u32 *)(h + 0x10) | 0x10;
        func_02026364(r4);
        if (*(u32 *)(h + 0x4c) & 0x200)
            (*(void (**)(u8 *, u32))(h + 0x48))(a0, 9);
        b = (*(u32 *)(a0 + 0xc) & 4) != 0;
        if (b == 0) {
            func_020293a4(a0);
            return 1;
        }
    } else {
        b = (*(u32 *)(a0 + 0xc) & 4) != 0;
        if (b == 0) {
            func_02026364(r4);
            return 1;
        }
        do {
            func_020251c0((u16 *)(a0 + 0x18));
        } while (a0 != *(u8 **)(h + 0x18));
        func_02026364(r4);
    }
    return func_0202935c(a0);
}