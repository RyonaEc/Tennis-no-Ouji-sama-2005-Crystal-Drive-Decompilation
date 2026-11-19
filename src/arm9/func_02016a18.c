#include "types.h"

extern void func_020176f0(void);
extern void func_02017544(void);
extern s32 func_020169dc(void);

void func_02016a18(u8 *p)
{
    if (*(u16 *)(p + 2)) {
        func_020176f0();
        func_02017544();
        return;
    }
    if (func_020169dc())
        return;
    func_02017544();
}