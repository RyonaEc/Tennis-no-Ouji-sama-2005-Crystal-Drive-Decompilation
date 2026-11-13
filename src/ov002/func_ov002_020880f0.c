#include "types.h"

extern u8 data_ov002_02102070[];
extern void func_ov002_0208804c(void);
void func_ov002_020880f0(void)
{
    u8 *p = *(u8 **)data_ov002_02102070;
    u8 a = p[0x1fe];
    p[0x1fe] = p[0x1ff];
    p[0x1ff] = a;
    func_ov002_0208804c();
}
