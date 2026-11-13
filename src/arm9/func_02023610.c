#include "types.h"

extern u32 func_02026710(u32, u32);
extern u32 func_02021000(u32);
extern u8 data_02042370[];
extern u8 data_02048c9c[];
extern u8 data_02048ca0[];
extern u8 data_02048ca4[];

u32 func_02023610(void)
{
    u32 t0, t1;
    if (*(u32 *)(data_02042370) != (-1)) {
        t0 = func_02026710(*(u32 *)(data_02042370), (-1));
    }
    t1 = func_02021000(*(u32 *)(data_02048ca4));
    *(u32 *)(data_02048ca4) = 0x0;
    *(u32 *)(data_02048ca0) = 0x0;
    *(u32 *)(data_02048c9c) = 0x0;
    return (u32)data_02048c9c;
}
