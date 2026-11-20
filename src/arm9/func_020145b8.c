#include "types.h"

extern u8 data_02042a68[];
extern u8 *func_02014d34(void);

s32 func_020145b8(s32 a0, s32 a1) {
    s32 i;
    u8 *b;
    u8 *t;
    u8 v;

    for (i = a1; i < 4; i++) {
        b = *(u8 **)(data_02042a68 + 0x14);
        t = b + i;
        if (t[0x24] == 0xff)
            continue;
        v = *(u8 *)(func_02014d34() + 1);
        if (v != a0)
            return 0;
    }
    return 1;
}