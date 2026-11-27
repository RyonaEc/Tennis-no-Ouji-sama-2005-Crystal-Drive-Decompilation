#include "types.h"

extern u16 *func_ov003_02053b30(void);
extern u32 func_ov003_02053b08(u32 i);

s32 func_ov003_02051b30(u8 *p)
{
    u16 i;
    u16 *y;
    u16 *t;
    s32 st;

    func_ov003_02053b30();
    st = *(u16 *)(p + 0x86fc);
    if (st == 0) {
        for (i = 0; i < 1; i++) {
            y = (u16 *)func_ov003_02053b08(i & 0xff);
            if ((*(u16 *)((u8 *)y + 0x14) & 0xf) != 0 &&
                (*(u16 *)((u8 *)y + 0x14) & 0xf) < 5) {
                t = func_ov003_02053b30();
                *(u16 *)((u8 *)t + 0x16) |= *(u16 *)((u8 *)y + 0x14) & 0xf;
                *(u16 *)(p + 0x86fc) = 0xa;
                i = 0xa;
            }
        }
    } else {
        if (st != 0) {
            if (st == 1) {
                t = func_ov003_02053b30();
                *(u16 *)((u8 *)t + 0x16) &= 0xf0;
            }
            *(u16 *)(p + 0x86fc) -= 1;
        }
    }
    return 0;
}