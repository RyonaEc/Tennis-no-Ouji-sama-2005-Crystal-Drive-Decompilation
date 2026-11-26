#include "types.h"

extern u8 data_ov004_020690a8[];
extern u8 data_ov004_02067ef0[];

extern u32 func_ov004_0206256c(s32 a0);
extern void func_ov004_020624e0(s32 a0, s32 a1);
extern u32 func_0200fd40(void);

s32 func_ov004_020622c4(void)
{
    u8 *g;
    s32 v;

    if (func_ov004_0206256c(9) == 0) {
        g = *(u8 **)data_ov004_020690a8;
        if (*(u32 *)(g + 8) == 1)
            return 0;
        if (func_ov004_0206256c(7) != 0) {
            if (func_ov004_0206256c(8) == 0)
                return 0;
            func_ov004_020624e0(9, 1);
            return 3;
        }
        v = func_0200fd40();
        return (v >= *(s32 *)(data_ov004_02067ef0 + 4)) ? 1 : 2;
    }
    return 0;
}