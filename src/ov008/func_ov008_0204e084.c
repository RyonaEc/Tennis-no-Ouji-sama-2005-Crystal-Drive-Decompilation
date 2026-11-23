#include "types.h"

extern void func_02026a04(u32, u32, u32);
extern void func_02020af0(u32);
extern void func_020205f8(void);
extern void func_02020648(void);
extern void func_0202065c(void);
extern void func_020206b8(void);
extern void func_020206cc(void);
extern void func_0202060c(void);

void func_ov008_0204e084(void)
{
    volatile u32 b;
    volatile u32 d;
    volatile u32 a;
    volatile u32 c;
    volatile u32 e;

    func_020206b8();
    func_020206cc();
    func_0202065c();
    func_02020648();
    func_020205f8();
    func_02020af0(0x1ff);
    a = 0;
    func_02026a04(a, 0x6800000, 0xa4000);
    func_0202060c();
    b = 0xc0;
    func_02026a04(b, 0x7000000, 0x400);
    c = 0;
    func_02026a04(c, 0x5000000, 0x400);
    d = 0xc0;
    func_02026a04(d, 0x7000400, 0x400);
    e = 0;
    func_02026a04(e, 0x5000400, 0x400);
}