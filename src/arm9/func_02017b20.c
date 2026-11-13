#include "types.h"

extern void func_02018128(void *, s32);
extern void func_02018100(void *, s32);
extern void func_02017bfc(void *, void *);
void func_02017b20(u8 *a0, s32 a1)
{
    u8 buf[12];
    u8 *p = a0 + 0x24;
    s32 n = a1 - 0x10;
    func_02018128(buf, n);
    func_02018100(p + 8, n);
    func_02017bfc(p, buf);
}
