#include "types.h"

extern u32 func_02014d34(void);

extern u8 data_02042a68[];

u32 func_02014618(u32 a0)
{
    s32 i;

    for (i = 1; i < 4; i++) {
        u8 id;
        u32 c = *(u8 *)(*(u32 *)(data_02042a68 + 0x14) + i + 0x24);

        if (c == 0xff)
            continue;
        id = *(u8 *)(func_02014d34() + 1);
        if (id != a0)
            return 0;
    }
    return 1;
}