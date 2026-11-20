#include "types.h"

s32 func_ov004_020500cc(u8 *a0, u32 a1, u32 a2)
{
    s32 n = *(s32 *)a0;
    s32 i;
    s32 ok;
    u32 id;

    for (i = 0; i < n; i++) {
        id = *((u32 *)a0 + i + 8);
        if (id == a1) {
            ok = ((s32 (*)(u32, s32, s32, s32, s32))*(u32 *)(a0 + 4))(a1,
                    *(s16 *)(a0 + 0x10), *(s16 *)(a0 + 0x12),
                    *(s16 *)(a0 + 0x14), *(s16 *)(a0 + 0x16));
            if (ok == 0)
                return 0;
            *(u32 *)(a0 + 0x18) = a2;
            return 1;
        }
    }
    return 0;
}