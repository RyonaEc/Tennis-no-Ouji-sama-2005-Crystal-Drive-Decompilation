#include "types.h"

extern u8 data_ov010_0205e500[];
#define G (*(u8 **)data_ov010_0205e500)

void func_ov010_02052258(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        *(u16 *)(G + i * 0x74 + 0x1e0) = 0;
        *(u16 *)(G + i * 0x74 + 0x1e2) = 0;
        *(u16 *)(G + i * 0x74 + 0x1e4) = 0;
        *(u16 *)(G + i * 0x74 + 0x21c) = 0;
    }
}