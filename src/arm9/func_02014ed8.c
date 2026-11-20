#include "types.h"

extern u8 data_02042a68[];
extern s32 func_02017360(void);
extern void func_02014f68(void);

s32 func_02014ed8(void)
{
    u8 *p = *(u8 **)(*(u8 **)(data_02042a68 + 0x14) + 0x104);
    s32 v;
    u16 a;

    *(u8 *)p = 0;
    *(u8 *)(p + 1) = 0;
    *(u16 *)(p + 8) = ~*(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1e);
    *(u16 *)(p + 6) = *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1e);
    *(u16 *)(p + 2) = *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1c);
    v = func_02017360();
    a = *(u16 *)(*(u8 **)(data_02042a68 + 0x14) + 0x1e);
    if (a == v) {
        func_02014f68();
        *(u8 *)(p + 1) = 2;
        return 1;
    }
    if (a > (a & v)) return 2;
    return 0;
}