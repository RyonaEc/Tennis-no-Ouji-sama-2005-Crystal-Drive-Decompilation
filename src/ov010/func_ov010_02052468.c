#include "types.h"

extern u8 data_ov010_0205e500[];

void func_ov010_02052468(void)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        ((u32 *)(*(u8 **)data_ov010_0205e500 + 0x164))[i] = 0xc;
        ((u32 *)(*(u8 **)data_ov010_0205e500 + 0x184))[i] = 0xc;
    }
}
