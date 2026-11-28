#include "types.h"

extern u16 data_020422f4;
extern u8 data_02042b28[];
extern u8 data_02042300[];
extern u32 func_0202e7c0(void);
extern u32 func_02017710(u32 a0);
extern s32 func_020304a8(s32 a0, s32 a1, s32 a2, s32 a3, u16 a4);
extern void func_020176f0(u32 a0);
extern u32 func_02016fb4(void);
extern void func_02017010(void);

u32 func_020170d0(void)
{
    u32 v;
    u32 i;
    s32 r;

    v = func_0202e7c0();
    if (v == 0) {
        func_02017710(7);
        return 0;
    }
    for (i = data_020422f4; i < 0xe; i = data_020422f4) {
        if (v & (1 << (i - 1))) {
            r = func_020304a8((s32)func_02017010, 3, 0x11, i, 0x1e);
            if (r == 2)
                return 1;
            func_020176f0(r);
            return 0;
        }
        data_020422f4++;
    }
    i = *(u16 *)(data_02042b28 + 8);
    if (i != 0xffff) {
        *(u16 *)(data_02042300 + 0x32) = i;
        func_02017710(1);
        func_02016fb4();
        return 1;
    }
    func_02017710(7);
    return 0;
}