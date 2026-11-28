#include "types.h"

extern u8 data_02042a68[];
extern u32 func_0201745c(void);
extern u32 func_02014d34(u32 a0);
extern u32 func_02016094(void);
extern void func_02014f68(void);
extern void func_02026564(u8 *out);
extern void func_020269d0(u8 *src, u8 *dst, s32 count);
extern u32 func_0201746c(void);
extern u32 func_02017544(void);

s32 func_02014d50(void)
{
    u8 buf[0x54];
    u8 *p;
    u8 *q;
    u32 r4;

    switch (func_0201745c()) {
    case 1:
    case 2:
    case 7:
        r4 = 2;
        break;
    case 5:
        r4 = func_02014d34(0);
        if (r4 == 0) {
            r4 = 0;
            break;
        }
        if (*(u16 *)(r4 + 8) & (1 << func_02016094())) {
            r4 = 2;
            break;
        }
        if (*(u8 *)(r4 + 1) != 2) {
            r4 = 0;
            break;
        }
        *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1c) = *(u16 *)(r4 + 2);
        *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1e) = *(u16 *)(r4 + 6);
        func_02014f68();
        r4 = 1;
        break;
    default:
        r4 = 0;
        break;
    }
    p = *(u8 **)(data_02042a68 + 0x14);
    q = *(u8 **)(p + 0x104);
    q[0] = 0;
    q[1] = 1;
    func_02026564(buf);
    *(u16 *)(q + 2) = *(u16 *)(buf + 0x18);
    func_020269d0(buf + 4, q + 4, 0x14);
    if (r4 == 2 && func_0201746c() == 0)
        func_02017544();
    return r4;
}