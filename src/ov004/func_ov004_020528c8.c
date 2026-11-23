#include "types.h"

extern u32 func_02017b20(u32, u32);
extern u8 data_02042a68[];

typedef struct {
    u32 f0x0;
    u8 pad_4[108];
    u32 f0x70;
    u8 pad_116[8];
    u32 f0x7c;
    u8 pad_128[8];
    u32 f0x88;
    u8 pad_140[8];
    u32 f0x94;
    u32 f0x98;
} S_ov004_020528c8;

u32 func_ov004_020528c8(u32 a0)
{
    u32 t0, t1, t2;
    if (((S_ov004_020528c8 *)a0)->f0x94 != 0x0) {
        t0 = func_02017b20(*(u32 *)(data_02042a68), ((S_ov004_020528c8 *)a0)->f0x94);
    }
    if (((S_ov004_020528c8 *)a0)->f0x7c != 0x0) {
        t1 = func_02017b20(*(u32 *)(data_02042a68), ((S_ov004_020528c8 *)a0)->f0x7c);
    }
    if (((S_ov004_020528c8 *)a0)->f0x88 != 0x0) {
        t2 = func_02017b20(*(u32 *)(data_02042a68), ((S_ov004_020528c8 *)a0)->f0x88);
    }
    ((S_ov004_020528c8 *)a0)->f0x94 = 0x0;
    ((S_ov004_020528c8 *)a0)->f0x7c = 0x0;
    ((S_ov004_020528c8 *)a0)->f0x88 = 0x0;
    ((S_ov004_020528c8 *)a0)->f0x98 = 0x0;
    ((S_ov004_020528c8 *)a0)->f0x0 = 0x0;
    ((S_ov004_020528c8 *)a0)->f0x70 = 0x0;
    return 0x0;
}
