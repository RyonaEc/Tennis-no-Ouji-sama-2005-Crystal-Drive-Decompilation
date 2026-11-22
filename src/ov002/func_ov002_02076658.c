#include "types.h"

extern u8 data_ov002_02102074[];
extern void func_ov002_02076358(void);
extern u8 *func_ov002_02077974(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);

s32 func_ov002_02076658(s32 a0, s32 a1)
{
    u8 *g = data_ov002_02102074;
    u8 *r;

    if (a0 >= *(u16 *)(g + 0x38))
        return 0;
    r = func_ov002_02077974((u32)func_ov002_02076358, 0x14, 0, 0, 0);
    if (r != 0) {
        *(u8 *)(r + 6) = 1;
        *(u8 *)(r + 7) = a0;
        *(u8 *)(r + 5) = a1;
    }
}