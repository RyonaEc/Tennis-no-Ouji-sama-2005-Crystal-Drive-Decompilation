#include "types.h"

extern u16 *func_ov003_02053b30(void);
extern u32 func_ov003_02053b08(u32 i);

s32 func_ov003_02051a68(u8 *p)
{
    u16 *x;
    u16 *y;
    s32 st = *(u16 *)(p + 0x86fe);

    if (st == 0) {
        x = func_ov003_02053b30();
        *(u16 *)((u8 *)x + 0x14) = 0;
    } else if (st == 1) {
        x = func_ov003_02053b30();
        if ((*(u16 *)((u8 *)x + 0x14) & 0xf) != 0 &&
            (*(u16 *)((u8 *)x + 0x14) & 0xf) < 5) {
            y = func_ov003_02053b30();
            if (((*(u16 *)((u8 *)y + 0x14) + 0x1d) & 0xf) ==
                ((*(u16 *)((u8 *)func_ov003_02053b08(0) + 0x16) + 0x1d) & 0xf))
                *(u16 *)((u8 *)y + 0x14) = 0;
        } else {
            *(u16 *)((u8 *)x + 0x14) = 0;
        }
        *(u16 *)(p + 0x86fe) = 0;
    } else {
        if (*(u16 *)(p + 0x86fe) != 0)
            *(u16 *)(p + 0x86fe) -= 1;
    }
    return 0;
}