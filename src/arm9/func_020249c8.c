#include "types.h"

extern u8 data_02048ccc[];
extern u32 data_02042384;

u32 func_020249c8(u32 x)
{
    s32 i;
    u32 *p = &data_02042384;

    for (i = 0; i < 0x16; i++) {
        if (x & 1) {
            if (i >= 8 && i <= 0xb)
                return *(u32 *)(data_02048ccc + (i - 8) * 0xc);
            if (i >= 3 && i <= 6)
                return *(u32 *)(data_02048ccc + (i + 1) * 0xc);
            return *p;
        }
        x >>= 1;
        p++;
    }
    return 0;
}