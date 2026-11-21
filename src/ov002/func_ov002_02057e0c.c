#include "types.h"

extern u8 *data_ov002_02102070;
extern void func_0203447c(u8 *, u32, u32);

void func_ov002_02057e0c(void)
{
    u8 *p = data_ov002_02102070;
    u8 *q = p + 0x1934;

    func_0203447c(p + 0x1824, 0, 0x110);
    func_0203447c(q, 0, 0x500);
}