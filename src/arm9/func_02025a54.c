#include "types.h"

extern u8 data_02048d3c[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);

s32 func_02025a54(u8 *a0)
{
    u32 s = func_02026350();
    u32 g = *(u32 *)(data_02048d3c + 8);
    s32 r;

    if (*(u32 *)(a0 + 4) == 0) {
        *(u32 *)(a0 + 4) = g;
        *(u32 *)(a0 + 8) += 1;
        r = 1;
    } else if (*(u32 *)(a0 + 4) == g) {
        *(u32 *)(a0 + 8) += 1;
        r = 1;
    } else
        r = 0;
    func_02026364(s);
    return r;
}