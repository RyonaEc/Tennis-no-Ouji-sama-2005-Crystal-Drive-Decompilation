#include "types.h"

extern void func_020176f0(void);
extern void func_02017710(u32 a0);
extern void func_02017544(void);
extern s32 func_02016a64(void);

void func_02016aa8(u8 *p)
{
    if (*(u16 *)(p + 2)) {
        func_020176f0();
        func_02017710(7);
        func_02017544();
        return;
    }
    if (func_02016a64())
        return;
    func_02017710(7);
    func_02017544();
}