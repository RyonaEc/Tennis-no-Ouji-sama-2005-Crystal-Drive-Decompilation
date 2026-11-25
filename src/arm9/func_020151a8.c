#include "types.h"

extern u8 data_02042a68[];
extern u32 func_0201745c(void);
extern u32 func_020160cc(void);
extern void func_02015e64(s32 a0);
extern u32 func_02017544(void);

void func_020151a8(void)
{
    u8 *g;
    u8 *p;

    switch (func_0201745c()) {
    case 1:
        g = data_02042a68;
        p = (u8 *)(*(u32 *)(g + 0x14) + 0x1a);
        *(u16 *)p = *(u16 *)p ^ 1;
        if (*(u16 *)(*(u32 *)(g + 0x14) + 0x1a) != 0 && func_020160cc() != 0)
            func_02015e64(2);
        else
            func_02015e64(1);
        break;
    case 7:
        func_02017544();
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        break;
    }
}