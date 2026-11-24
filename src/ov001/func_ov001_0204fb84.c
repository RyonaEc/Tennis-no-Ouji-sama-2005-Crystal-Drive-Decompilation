#include "types.h"

extern u32 func_020051a0(u32, u32);
extern u32 func_020051b8(u32, u32);
extern u32 func_02021b14(u32, u32, u32, u32, u32);
extern u8 data_ov001_0204fe18[];
extern u8 data_ov001_02051cc0[];

u32 func_ov001_0204fb84(u32 a0)
{
    u32 sl0;

    u32 t0, t1, t2, t3;

    if (a0 != 0x0) {
        t0 = func_020051a0(0x2, 0x12);
        t1 = func_020051b8((u32)(u32)data_ov001_0204fe18, 0x3);
        *(u8 *)((u8 *)((*(u32 *)(data_ov001_02051cc0) + 0xa))) = (*(u8 *)((u8 *)((*(u32 *)(data_ov001_02051cc0) + 0xa))) | 0x2);
    } else {
        t2 = func_020051b8(0x0, 0x0);
        *(u8 *)((u8 *)((*(u32 *)(data_ov001_02051cc0) + 0xa))) = (*(u8 *)((u8 *)((*(u32 *)(data_ov001_02051cc0) + 0xa))) & ~0x2);
    }

    t3 = func_02021b14(0x4001050, 0x0, 0xc, 0x10, 0x0);

    sl0 = 0x0;

    return t3;

}

