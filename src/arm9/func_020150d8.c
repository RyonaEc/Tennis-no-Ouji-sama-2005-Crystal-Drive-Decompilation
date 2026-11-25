#include "types.h"

typedef struct {
    u8 pad0[0x14];
    u32 f14;
} S_02042a68;

extern S_02042a68 data_02042a68;
extern u32 func_0201745c(void);
extern u32 func_02017550(u32 a0);
extern u32 func_020160f0(void);
extern u32 func_02017370(void);
extern u32 func_02017544(void);
extern u32 func_02017360(void);

void func_020150d8(void)
{
    u8 *p;
    u32 base;

    switch (func_0201745c()) {
    case 0:
    case 3:
    case 4:
    case 5:
    case 6:
        break;
    case 1:
        func_02017550(4);
        break;
    case 2:
        func_020160f0();
        break;
    case 7:
        if (func_02017370() == 0x15)
            func_02017544();
        break;
    }
    base = data_02042a68.f14;
    p = *(u8 **)(base + 0x104);
    p[0] = 0;
    p[1] = 0;
    *(u16 *)(p + 4) = (u16)func_02017360();
    *(u16 *)(data_02042a68.f14 + 0x1e) &= *(u16 *)(p + 4);
    *(u16 *)(p + 6) = *(u16 *)(data_02042a68.f14 + 0x1e);
    *(u16 *)(p + 8) = 0;
}