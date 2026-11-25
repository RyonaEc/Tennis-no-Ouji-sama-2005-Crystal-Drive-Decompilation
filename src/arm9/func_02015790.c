#include "types.h"

extern u16 data_02042af0;
extern u32 data_02042b00;
extern u32 func_02015574(void);
extern u32 func_02016030(u32 a0);
extern u32 func_02017544(void);
extern u32 func_020176f0(u32 a0);
extern u32 func_02017710(u32 a0);

void func_02015790(u8 *a0)
{
    if (*(u16 *)(a0 + 2) != 0) {
        func_020176f0(*(u16 *)(a0 + 2));
        return;
    }
    if (func_02015574() != 0 && data_02042b00 != 0) {
        if (func_02016030(9) != 0)
            return;
        func_02017544();
        return;
    }
    if (data_02042af0 == 3) {
        if (func_02016030(8) == 0) {
            func_020176f0(0x17);
            func_02017710(7);
        }
        data_02042af0 = 0;
        return;
    }
    func_02017544();
}