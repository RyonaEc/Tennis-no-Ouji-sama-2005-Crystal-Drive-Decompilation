#include "types.h"

extern u8 data_ov004_020690a0[];
extern u8 *data_ov004_020690a8;
extern s32 func_ov004_0204dcb4(u8 *p, u32 v);
extern s32 func_ov004_0204dce4(u8 *a0, u16 a1);
extern u32 func_0200649c(u32 a0);

s32 func_ov004_0205a8cc(u32 a0)
{
    u8 *b = (u8 *)(*(u32 *)data_ov004_020690a0 + 0x13a8);
    u8 *g = data_ov004_020690a8;
    s32 r;

    if (*(u32 *)(*(u32 *)(b + 4)) != 0) {
        return 0;
    }
    if (*(u32 *)(b + 0x14) != 0) {
        *(u32 *)(b + 0x14) = 0;
    }
    *(u32 *)(b + 0xc) = a0;
    *(u32 *)(b + 0x10) = a0;
    r = func_ov004_0204dcb4(*(u8 **)(b + 4),
                            *(u32 *)(*(u32 *)(b + 8) + *(u32 *)(b + 0xc) * 4));
    if (*(u32 *)(b + 0xc) == 1) {
        switch (*(u32 *)(g + 8)) {
        case 0:
        default:
            func_0200649c(9);
            break;
        case 1:
            func_0200649c(0xa);
            break;
        }
        *(u32 *)(b + 0x18) = 1;
        if (r != 0) {
            return func_ov004_0204dce4(*(u8 **)(b + 4), 8);
        }
    } else if (r != 0) {
        return func_ov004_0204dce4(*(u8 **)(b + 4), 2);
    }
    return 0;
}
