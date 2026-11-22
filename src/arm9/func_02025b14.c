#include "types.h"

extern u8 data_02048d3c[];
extern u32 func_02026350(void);
extern void func_02026364(u32 a0);
extern void func_02025a08(u32 a0, u8 *a1);
extern void func_0202514c(u8 *a0, u32 a1);

void func_02025b14(u8 *a0)
{
    u32 s = func_02026350();
    u32 g = *(u32 *)(data_02048d3c + 8);

    if (*(u32 *)(a0 + 4) == g) {
        *(u32 *)(a0 + 8) -= 1;
        if (*(u32 *)(a0 + 8) == 0) {
            func_02025a08(g, a0);
            *(u32 *)(a0 + 4) = 0;
            func_0202514c(a0, 0);
        }
    }
    func_02026364(s);
}