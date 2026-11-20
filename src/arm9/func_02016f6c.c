#include "types.h"

extern void func_02017710(u32);
extern void func_020176f0(u32);
extern u32 func_02016ee8(void);
extern void func_02017544(void);

void func_02016f6c(u8 *a0)
{
    if (*(u16 *)(a0 + 2) != 0)
    {
        func_02017710(7);
        func_020176f0(*(u16 *)(a0 + 2));
        return;
    }
    if (func_02016ee8() != 0) return;
    func_02017544();
}