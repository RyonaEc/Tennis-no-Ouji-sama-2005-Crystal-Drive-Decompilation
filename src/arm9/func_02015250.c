#include "types.h"

extern s32 func_0201745c(void);
extern void func_02015e64(s32);
extern void func_02017544(void);
extern void func_0201746c(void);

extern u8 data_02042a68[];

s32 func_02015250(void)
{
    func_0201745c();
    switch (func_0201745c()) {
    case 1:
        *(u16 *)(*(u32 *)(data_02042a68 + 0x14) + 0x1e) = 1;
        *(u16 *)(*(u32 *)(data_02042a68 + 0x14) + 0x1a) = 0;
        func_02015e64(0);
        return 1;
    case 7:
        func_02017544();
        break;
    case 5:
        func_0201746c();
        break;
    }
    return 0;
}