#include "types.h"

extern u8 data_ov002_02102070[];
extern u32 func_ov002_020848a0(void);
extern void func_ov002_0207b494(void);
extern void func_ov002_0208a034(void);
extern void func_ov002_0207b504(void);
extern void func_ov002_02081c60(void);
extern u32 func_ov002_02081078(u32 a0, s32 a1);
extern void func_ov002_0207fe90(u32 a0);
extern void func_ov002_02081180(void);

void func_ov002_02081cfc(void)
{
    u8 *b = *(u8 **)data_ov002_02102070;
    u8 *a = b + 0x3654;
    u8 *c = b + 0x1e34;
    u32 r;

    func_ov002_020848a0();
    if (*(s32 *)(b + 0x28) == 0) {
        func_ov002_0207b494();
        func_ov002_0208a034();
        if (*(u8 *)(b + 0x100) == 0 && !(*(s32 *)(c + 0x34) & 2)) {
            func_ov002_0207b504();
            *(s32 *)(a + 0x10) &= ~0x18;
            func_ov002_02081c60();
            if (!(*(s32 *)(a + 0x10) & 9))
                func_ov002_02081078(0, *(s32 *)(a + 0x14));
            r = *(s32 *)(a + 0x10) & 0x12;
            if (r == 0)
                r = func_ov002_02081078(1, *(s32 *)(a + 0x18));
            func_ov002_0207fe90(r);
        }
    }
    func_ov002_02081180();
    {
        u8 *cnt = a + 8;

        *cnt = *cnt + 1;
    }
    if (a[8] >= 0xc)
        a[8] = 0;
}