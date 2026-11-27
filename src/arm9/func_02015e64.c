#include "types.h"

extern u32 data_02042b00;
extern u8 data_02042fa0[];
extern u16 data_02042af4;

u32 func_02017710(u32 a0);
s32 func_02015e14(u32 a0);
void func_02026a04(u32 data, void *destp, u32 size);
void func_02025c94(u32 a0, u32 a1);

s32 func_02015e64(u32 sel) {
    s32 i;
    u8 *p;
    volatile u32 z;

    func_02017710(3);
    switch (sel) {
    case 0:
        data_02042b00 = 0;
        p = data_02042fa0;
        for (i = 0; i < 8; i++) {
            z = 0;
            func_02026a04(z, p, 0xc0);
            p += 0xc0;
        }
        func_02025c94((u32)data_02042fa0, 0x600);
        data_02042af4 = 1;
        /* fallthrough */
    case 1:
        return func_02015e14(1);
    case 2:
        return func_02015e14(2);
    case 3:
        return func_02015e14(3);
    default:
        return 0;
    }
}