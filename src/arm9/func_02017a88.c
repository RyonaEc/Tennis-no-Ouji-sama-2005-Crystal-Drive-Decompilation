#include "types.h"

extern u8 *func_02017784(u8 *, u8 *);

u8 *func_02017a88(u8 *a0, u32 a1)
{
    u8 *n = func_02017784(a0, 0);

    while (n != 0) {
        if (*(u32 *)(n + 0x18) <= a1 && a1 < *(u32 *)(n + 0x1c)) {
            u8 *r = func_02017a88(n + 0xc, a1);
            if (r != 0) return r;
            return n;
        }
        n = func_02017784(a0, n);
    }
    return 0;
}