#include "types.h"

extern u8 data_ov004_020690a4[];
extern s32 func_02017b20(u32, u32);

s32 func_ov004_0204e034(void)
{
    s32 i;
    s32 off;
    s32 j;
    u32 handle;
    u8 *p;

    handle = *(u32 *)(*(u8 **)data_ov004_020690a4 + 8);
    for (i = 0, off = 0; i < 6; i++, off += 0x18) {
        if (*(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x10) != 0) {
            func_02017b20(handle, *(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x10));
            *(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x10) = 0;
        }
        if (*(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x14) != 0) {
            func_02017b20(handle, *(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x14));
            *(u32 *)(*(u8 **)data_ov004_020690a4 + off + 0x14) = 0;
        }
    }
    for (j = 0; j < (s32)*(u32 *)(*(u8 **)data_ov004_020690a4 + 4); j++) {
        p = (u8 *)(*(u32 *)(*(u8 **)data_ov004_020690a4 + 0x9c)) + j * 0x1c0;
        if (*(u32 *)p == 0)
            continue;
        if (*(u32 *)(p + 0x20) != 0) {
            func_02017b20(handle, *(u32 *)(p + 0x20));
            *(u32 *)(p + 0x20) = 0;
        }
        if (*(u32 *)(p + 0x24) != 0) {
            func_02017b20(handle, *(u32 *)(p + 0x24));
            *(u32 *)(p + 0x24) = 0;
        }
        if (*(u32 *)(p + 0x28) != 0) {
            func_02017b20(handle, *(u32 *)(p + 0x28));
            *(u32 *)(p + 0x28) = 0;
        }
    }
    if (*(u32 *)(*(u8 **)data_ov004_020690a4 + 0x9c) != 0)
        func_02017b20(handle, *(u32 *)(*(u8 **)data_ov004_020690a4 + 0x9c));
    if (*(u8 **)data_ov004_020690a4 != 0)
        func_02017b20(handle, (u32)*(u8 **)data_ov004_020690a4);
    *(u32 *)data_ov004_020690a4 = 0;
    return 1;
}