#include "types.h"

typedef struct {
    u32 w[6];
} B24;

typedef struct {
    u16 f0;
    u16 f2;
    u32 f4;
    u32 f8;
} E_020466b8;

extern u8 *data_02042360;
extern u8 *data_02042358;
extern u32 *data_02042364;
extern u8 data_02046e34[];
extern E_020466b8 data_020466b8[];
extern u16 func_0201a04c(void);

void func_0201987c(void)
{
    u16 old;
    u16 now;

    old = func_0201a04c();
    if ((u32)(data_02042360 + 0x18) < (u32)data_02046e34) {
        *(B24 *)(data_02042360 + 0x18) = *(B24 *)data_02042360;
        *(B24 *)(data_02042358 + 0x18) = *(B24 *)data_02042358;
        data_02042364[1] = data_02042364[0];
        data_02042360 = data_02042360 + 0x18;
        data_02042358 = data_02042358 + 0x18;
        data_02042364 = data_02042364 + 1;
    }
    now = func_0201a04c();
    data_020466b8[now].f0 = data_020466b8[old].f0;
    data_020466b8[now].f4 = data_020466b8[old].f4;
}