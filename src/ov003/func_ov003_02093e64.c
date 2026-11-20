#include "types.h"

extern s32 func_0203b0c0(s32 a, s32 b);

u16 func_ov003_02093e64(s32 a0, s32 a1) {
    s32 x;
    s32 y;

    if (a0 == 0) {
        x = 0x1e;
        y = -0xf;
    } else if (a0 == 1) {
        x = 0x1e;
        y = 0xf;
    } else if (a0 == 2) {
        x = 0x1e;
        y = -0xf;
    } else {
        x = 0x1e;
        y = 0xf;
    }
    if (x == 0)
        return 0;
    return (u16)func_0203b0c0(y * a1, x);
}